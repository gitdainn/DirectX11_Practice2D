#include "stdafx.h"
#include "Player.h"
#include "PlayerJump.h"
#include "PlayerIdle.h"

CPlayerJump::CPlayerJump()
	: m_bIsDead(false)
	, m_iMaxJumpCount(2), m_iJumpCount(0)
	, m_UpTime(0.0), m_DownTime(0.0)
	, m_JumpTimeAcc(0.0)
	, m_bIsPaused(false)
{
}

CPlayerJump::CPlayerJump(const _uint& iJumpCount)
	: m_iJumpCount(iJumpCount)
	, m_bIsDead(false)
	, m_iMaxJumpCount(2)
	, m_UpTime(0.0), m_DownTime(0.0)
	, m_JumpTimeAcc(0.0)
	, m_bIsPaused(false)
{
}

CPlayerJump::CPlayerJump(const CPlayerJump& rhs)
	: m_bIsDead(rhs.m_bIsDead)
	, m_iMaxJumpCount(rhs.m_iMaxJumpCount), m_iJumpCount(rhs.m_iJumpCount)
	, m_UpTime(rhs.m_UpTime), m_DownTime(rhs.m_DownTime)
	, m_JumpTimeAcc(rhs.m_JumpTimeAcc)
	, m_bIsPaused(rhs.m_bIsPaused)
{
}

CPlayerJump::~CPlayerJump()
{
}

CState* CPlayerJump::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
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
		// 점프 시 MaxCount만 아니면 추가 점프 가능함.
		// 점프는 
		if (m_bIsDead)
		{
			pState = new CPlayerJump();
		}
		else if(m_iMaxJumpCount <= m_iJumpCount
			|| STATE_TYPE::DASH != eState && STATE_TYPE::JUMP != eState)
		{
			m_bIsPaused = false;
			pState = new CPlayerJump(*this);
		}
		else
		{
			pState = new CPlayerJump(++m_iJumpCount);
		}
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerJump::Enter(CSpriteObject* pObject)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
	if (nullptr != pPlayer)
		pPlayer->Set_IsInAir(true);

	pObject->Change_Sprite(STATE_TYPE::JUMP);
	++m_iJumpCount;
}

void CPlayerJump::Update(CSpriteObject* pObject, const _double TimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);

	if (nullptr == pObject || nullptr == pPlayer)
	{
		MSG_BOX("CPlayerJump - Update - Pointer is NULL");
		return;
	} 

	if (m_bIsFalling)
	{
		if (IsOnGround(pObject))
		{
			if (nullptr != pPlayer)
				pPlayer->Set_IsInAir(false);

			pObject->Input_Handler(STATE_TYPE::IDLE, pObject->Get_SpriteDirection());
			m_bIsDead = true;
			return;
		}
	}

	// 만약 땅에 닿았으면 끝.
	if (m_bIsPaused)
		return;
	
	Parabola(pObject, TimeDelta);
}

void CPlayerJump::Parabola(CSpriteObject* pObject, const _double TimeDelta)
{
	const _float fGravity = 9.8f;
	const _float fPower = 900.f;
	// sin(포물선운동 진행각도90) = 1
	m_UpTime = fPower * m_JumpTimeAcc * (_float)TimeDelta;
	m_DownTime = (fGravity * m_JumpTimeAcc * m_JumpTimeAcc) * 0.5f;

	_float fJumpY = m_UpTime - m_DownTime;
	m_JumpTimeAcc += TimeDelta;
	
	if (m_UpTime < m_DownTime)
		m_bIsFalling = true;
	else
		m_bIsFalling = false;

	CTransform* pTransform = pObject->Get_TransformCom();
	_vector vPlayerPos = pTransform->Get_State(CTransform::STATE_POSITION);

	/** @qurious - 포물선 점프로 얻은 값을 증가되는 현재 Y위치에 더해줘야함.. 왜지? */
	pTransform->Set_State(CTransform::STATE_POSITION
		, XMVectorSetY(vPlayerPos, fJumpY + XMVectorGetY(vPlayerPos)));
}

const bool CPlayerJump::IsOnGround(CSpriteObject* pObject)
{
	const _float fGround = -111.f;

	CTransform* pTransform = pObject->Get_TransformCom();
	_float	fPlayerY = XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION));

	if(fGround > fPlayerY)
	{
		pTransform->Set_State(CTransform::STATE_POSITION
			, XMVectorSetY(pTransform->Get_State(CTransform::STATE_POSITION), fGround));
		return true;
	}
	return false;
}
