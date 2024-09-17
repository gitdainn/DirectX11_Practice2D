#include "stdafx.h"
#include "PlayerAtk.h"
#include "PlayerIdle.h"
#include "PlayerDash.h"

CState* CPlayerAtk::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		pState = new CPlayerIdle();
		break;

	// ATK이 연속으로 들어오면 Combo 증가, 연속이 아니면 다른 상태 거쳐서 오므로 Combo 초기화됨
	case STATE_TYPE::DEFAULT_ATK:
		// @note - 연산자 우선수위: 증감 연산자 우선, 삼항 연산자 (?)
		m_iCombo = m_iMaxCombo < ++m_iCombo ? m_iMaxCombo : m_iCombo;
		pState = new CPlayerAtk(*this);
		break;

	case STATE_TYPE::ATK1:
	case STATE_TYPE::ATK2:
		pState = new CPlayerAtk(*this);
		break;

	case STATE_TYPE::DASH:
		pState = new CPlayerDash(eDirection);
		break;

	default:
		break;
	}
	return pState;
}

void CPlayerAtk::Enter(CSpriteObject* pObject)
{
	// if ATK이나 JUMP 등이 아니면 교체
	STATE_TYPE eAnimType = m_eAttackType;
	static STATE_TYPE ePastType = eAnimType;

	if (STATE_TYPE::DEFAULT_ATK == eAnimType && 2 <= m_iCombo)
	{
		eAnimType = STATE_TYPE::ATK2;
	}
	else if (STATE_TYPE::DEFAULT_ATK == eAnimType)
	{
		eAnimType = STATE_TYPE::ATK1;
	}
	else
		pObject->Change_AnimType((_uint)eAnimType);

	pObject->Set_CurrentState(m_eAttackType);
	if (ePastType != eAnimType)
	{
		ePastType = eAnimType;
		pObject->Change_AnimType((_uint)eAnimType);
	}

}

void CPlayerAtk::Update(CSpriteObject* pObject, _double TimeDelta)
{
	// 만약 애니메이션 끝났으면
	if (pObject->IsEndSprite())
	{
		pObject->Input_Handler(STATE_TYPE::IDLE);
	}
}
