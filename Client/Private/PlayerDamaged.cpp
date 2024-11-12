#include "stdafx.h"
#include "PlayerDamaged.h"
#include "PlayerIdle.h"
#include "PlayerWalk.h"
#include "PlayerAtk.h"
#include "PlayerDash.h"

CState* CPlayerDamaged::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		pState =  new CPlayerIdle();
		break;

	case STATE_TYPE::WALK:
		pState = new CPlayerWalk(eDirection);
		break;

	case STATE_TYPE::DEFAULT_ATK:
	case STATE_TYPE::ATK1:
	case STATE_TYPE::ATK2:
		pState = new CPlayerAtk(Input);
		break;

	case STATE_TYPE::DASH:
		pState = new CPlayerDash(eDirection);
		break;

	case STATE_TYPE::JUMP:
		break;

	case STATE_TYPE::SKILL1:
		break;

	case STATE_TYPE::SKILL2:
		break;

	case STATE_TYPE::DAMAGED:
		pState = new CPlayerDamaged();
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerDamaged::Enter(CSpriteObject* pObject)
{
	if (nullptr == pObject)
		return;

	//pObject->Set_CurrentState(STATE_TYPE::DAMAGED);
	//pObject->Change_AnimType((_uint)STATE_TYPE::DAMAGED);

	CTransform* pTransform = pObject->Get_TransformCom();
	if (nullptr == pTransform)
	{
		pObject->Input_Handler(STATE_TYPE::IDLE, pObject->Get_SpriteDirection());
		return;
	}
}

void CPlayerDamaged::Update(CSpriteObject* pObject, _double TimeDelta)
{
	if (nullptr == pObject)
		return;

	CTransform* pTransform = pObject->Get_TransformCom();
	if (nullptr == pTransform)
	{
		pObject->Input_Handler(STATE_TYPE::IDLE, pObject->Get_SpriteDirection());
		return;
	}

	if (!m_bHasOnce)
	{
		m_vStartPosition = pTransform->Get_State(CTransform::STATE_POSITION);
		m_vEndPosition = XMVectorSetX(m_vStartPosition, XMVectorGetX(m_vStartPosition) + m_fKnockBackDistance);
		m_bHasOnce = true;
	}

	m_DamagedTimeAcc += TimeDelta * 2.f;
	if (1.f <= m_DamagedTimeAcc)
	{
		pObject->Input_Handler(STATE_TYPE::IDLE, pObject->Get_SpriteDirection());
		return;
	}

	_vector vLerpPos = XMVectorLerp(m_vStartPosition, m_vEndPosition, CUtil::GetT_FastToSlow(m_DamagedTimeAcc));
	pTransform->Set_State(CTransform::STATE_POSITION, vLerpPos);

	return;
}
