#include "stdafx.h"
#include "Enemy.h"
#include "EnemyJump.h"
#include "LineRider.h"

CEnemyJump::CEnemyJump()
	: m_bIsDead(false)
	, iMaxJumpCount(2), m_iJumpCount(0)
	, m_UpTime(0.0), m_DownTime(0.0)
	, m_JumpTimeAcc(0.0)
	, m_bIsPaused(false)
	, m_fPower(500.f)
	, m_bCanJump(false), m_bIsFalling(false)
{
	Enter();
}

CEnemyJump::CEnemyJump(const _uint& iJumpCount)
	: m_iJumpCount(iJumpCount)
	, m_bIsDead(false)
	, iMaxJumpCount(2)
	, m_UpTime(0.0), m_DownTime(0.0)
	, m_JumpTimeAcc(0.0)
	, m_bIsPaused(false)
	, m_fPower(500.f)
	, m_bCanJump(false), m_bIsFalling(false)
{
}

CEnemyJump::CEnemyJump(const CEnemyJump& rhs)
	: m_bIsDead(rhs.m_bIsDead)
	, iMaxJumpCount(rhs.iMaxJumpCount), m_iJumpCount(rhs.m_iJumpCount)
	, m_UpTime(rhs.m_UpTime), m_DownTime(rhs.m_DownTime)
	, m_JumpTimeAcc(rhs.m_JumpTimeAcc)
	, m_bIsPaused(rhs.m_bIsPaused)
	, m_fPower(rhs.m_fPower)
{
}

CEnemyJump::~CEnemyJump()
{
}

void CEnemyJump::Enter()
{
	m_UpTime = 0.f;
	m_DownTime = 0.f;
	m_JumpTimeAcc = 0.f;

	++m_iJumpCount;
}

void CEnemyJump::Update(CAnimObject* pObject, const _double TimeDelta)
{
	if (nullptr == pObject)
	{
		MSG_BOX("CEnemyJump - Update - Pointer is NULL");
		return;
	}
	CEnemy* pEnemy = dynamic_cast<CEnemy*>(pObject);
	if (nullptr == pEnemy)
		return;

	CLineRider* pLineRiderCom = dynamic_cast<CLineRider*>(pObject->Get_Component(TAG_LINERIDER));
	if (nullptr == pLineRiderCom)
		return;

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
		//		m_bIsFalling = false;
		//		m_bIsDead = true;
		//		return;
		//	}
		//}
		//else
		{
			Parabola(pObject, TimeDelta);
			// 화면 밖이면 게임 엔드
		}
	}
	else
		Parabola(pObject, TimeDelta);
}

void CEnemyJump::Parabola(CAnimObject* pObject, const _double TimeDelta)
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
}

const _bool CEnemyJump::AttachToLineIfBelow(CAnimObject* pObject, const _float& fLandingY)
{
	if (nullptr == pObject)
	{
		MSG_BOX("CEnemyJump - IsOnGround() - NULL");
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

CEnemyJump* CEnemyJump::Create()
{
	CEnemyJump* pEnemyJump = new CEnemyJump();
	if (nullptr == pEnemyJump)
	{
		Safe_Delete(pEnemyJump);
		return nullptr;
	}

	return pEnemyJump;
}
