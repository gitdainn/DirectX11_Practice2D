#include "stdafx.h"
#include "Player.h"
#include "PlayerJump.h"
#include "PlayerIdle.h"
#include "PlayerDamaged.h"

CPlayerJump::CPlayerJump()
	: m_bIsDead(false)
	, iMaxJumpCount(2), m_iJumpCount(0)
	, m_UpTime(0.0), m_DownTime(0.0)
	, m_JumpTimeAcc(0.0)
	, m_bIsPaused(false)
	, m_fPower(500.f)
{
}

CPlayerJump::CPlayerJump(const CPlayerJump& rhs)
	: m_bIsDead(rhs.m_bIsDead)
	, iMaxJumpCount(rhs.iMaxJumpCount), m_iJumpCount(rhs.m_iJumpCount)
	, m_UpTime(rhs.m_UpTime), m_DownTime(rhs.m_DownTime)
	, m_JumpTimeAcc(rhs.m_JumpTimeAcc)
	, m_bIsPaused(rhs.m_bIsPaused)
	, m_fPower(rhs.m_fPower)
{
}

CPlayerJump::~CPlayerJump()
{
}

CState* CPlayerJump::Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };
	const STATE_TYPE eState = pObject->Get_CurrentState();
	switch (Input)
	{
	case STATE_TYPE::IDLE:
		m_bIsPaused = false;
		break;

	case STATE_TYPE::ATK1: // 공격 후에는 추가 점프 불가
		break;

	case STATE_TYPE::DASH:
	case STATE_TYPE::SKILL1:
	case STATE_TYPE::SKILL2:
		m_bIsPaused = true;
		break;

	case STATE_TYPE::JUMP:
		// 땅에 착지하면 m_bIsDead가 true가 됩니다.
		if (m_bIsDead)
		{
			pState = new CPlayerJump();
		}
		// 점프 횟수가 최대 점프 횟수 이상이면 땅에 착지하기 전까지 Jump 불가능
		else if (iMaxJumpCount < m_iJumpCount)
		{
			pState = nullptr;
		}
		else if(STATE_TYPE::DASH != eState && STATE_TYPE::JUMP != eState)
		{
			++m_iJumpCount;
			m_bIsPaused = false;
			pState = new CPlayerJump(*this);
		}
		else
		{
			++m_iJumpCount;
			pState = new CPlayerJump(*this);
		}
		break;

	case STATE_TYPE::DAMAGED:
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerJump::Enter(CPlayer* pObject)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	if (nullptr != pPlayer)
		pPlayer->Set_IsInAir(true);

	m_UpTime = 0.f;
	m_DownTime = 0.f;
	m_JumpTimeAcc = 0.f;

	++m_iJumpCount;
}

void CPlayerJump::Update(CPlayer* pObject, const _double TimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);

	if (nullptr == pObject || nullptr == pPlayer)
	{
		MSG_BOX("CPlayerJump - Update - Pointer is NULL");
		return;
	} 

	if (m_bIsFalling)
	{
		//// 플레이어 위치 이동 전 착지 가능한 땅을 먼저 검사한 후, 이동 후, 높이 비교해서 붙여야 함.
		//_float fLandingY = { 0.f };
		//if (HasPassableLine(pObject, fLandingY))
		//{
		//	Parabola(pObject, TimeDelta);

		//	// 착지할 땅보다 아래로 떨어지면 착지시키기.
		//	if (AttachToLineIfBelow(pObject, fLandingY))
		//	{
		//		if (nullptr != pPlayer)
		//			pPlayer->Set_IsInAir(false);

		//		STATE_TYPE eCurrentState = pObject->Get_CurrentState();
		//		if (STATE_TYPE::ATK1 != eCurrentState
		//			&& STATE_TYPE::ATK2 != eCurrentState
		//			&& STATE_TYPE::DEFAULT_ATK != eCurrentState
		//			&& STATE_TYPE::JUMP_ATK != eCurrentState)
		//		{
		//			pObject->Input_Handler(STATE_TYPE::IDLE, pObject->Get_SpriteDirection());
		//		}
		//		m_bIsFalling = false;
		//		m_bIsDead = true;
		//		return;
		//	}
		//}
		//else
		{
			Parabola(pObject, TimeDelta);
		}
	}
	else
		Parabola(pObject, TimeDelta);
}

void CPlayerJump::Parabola(CPlayer* pObject, const _double TimeDelta)
{
	const _float fGravity = 9.8f;
	const _float fJumpSpeed = 3.f;

	m_JumpTimeAcc += TimeDelta * fJumpSpeed;

	// sin(포물선운동 진행각도90) = 1
	m_UpTime = m_fPower * m_JumpTimeAcc * (_float)TimeDelta;
	m_DownTime = (fGravity * m_JumpTimeAcc * m_JumpTimeAcc) * 0.5f;

	_float fJumpY = (_float)(m_UpTime - m_DownTime);
	// @note - 전체적인 곡선 모양은 동일한데 속도만 빨리하고 싶다면 그래프상 시간을 빠르게 흐르게 하면 된다! (즉 시간만 속도 올려주기)
	
	if (m_UpTime < m_DownTime)
		m_bIsFalling = true;
	else
		m_bIsFalling = false;

	CTransform* pTransformCom = pObject->Get_TransformCom();
	if (nullptr == pTransformCom)
		return;

	_vector vPlayerPos = pTransformCom->Get_State(CTransform::STATE_POSITION);

	/** @qurious - 포물선 점프로 얻은 값을 증가되는 현재 Y위치에 더해줘야함.. 왜지? */
	pTransformCom->Set_State(CTransform::STATE_POSITION
		, XMVectorSetY(vPlayerPos, fJumpY + XMVectorGetY(vPlayerPos)));

	_vector vAfterMovePosition = pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float2 fScroll = _float2(0.f, abs(XMVectorGetY(vPlayerPos) - XMVectorGetY(vAfterMovePosition)));
	fScroll.y *= m_bIsFalling ? 1.f : -1.f;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	//pGameInstance->Set_Scroll(fScroll);
	//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());

	Safe_Release(pGameInstance);
}

_bool CPlayerJump::AttachToLineIfBelow(CPlayer* pObject, const _float& fLandingY)
{
	if (nullptr == pObject)
	{
		MSG_BOX("CPlayerJump - IsOnGround() - NULL");
		return false;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return false;
	Safe_AddRef(pGameInstance);

	CTransform* pTransformCom = pObject->Get_TransformCom();
	_vector vPosition = pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float2 vPlayerPos = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));

	if (fLandingY > XMVectorGetY(vPosition))
	{
		pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
		Safe_Release(pGameInstance);
		return true;
	}

	Safe_Release(pGameInstance);

	return false;
}
