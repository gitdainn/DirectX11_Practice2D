#include "stdafx.h"
#include "PlayerIdle.h"
#include "PlayerWalk.h"
#include "PlayerAtk.h"

CState* CPlayerIdle::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::WALK:
		pState = new CPlayerWalk();
		break;

	case STATE_TYPE::ATK:
		pState = new CPlayerAtk();
		break;

	case STATE_TYPE::DASH:
		break;

	case STATE_TYPE::JUMP:
		break;

	case STATE_TYPE::SKILL1:
		break;

	case STATE_TYPE::SKILL2:
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerIdle::Enter(CSpriteObject* pObject)
{
	// if ATK이나 JUMP 등이 아니면 교체
	pObject->Change_Sprite(STATE_TYPE::IDLE);
}

void CPlayerIdle::Update(CSpriteObject* pObject, _double TimeDelta)
{
	return;
}