#include "stdafx.h"
#include "PlayerIdle.h"
#include "PlayerWalk.h"
#include "PlayerAtk.h"
#include "PlayerDash.h"
#include "PlayerDamaged.h"

CState* CPlayerIdle::Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		return new CPlayerIdle(*this);

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

void CPlayerIdle::Enter(CPlayer* pObject)
{
	// if ATK이나 JUMP 등이 아니면 교체
	pObject->Set_CurrentState(STATE_TYPE::IDLE);
	pObject->Change_AnimType((_uint)STATE_TYPE::IDLE);
}

void CPlayerIdle::Update(CPlayer* pObject, _double TimeDelta)
{
	return;
}
