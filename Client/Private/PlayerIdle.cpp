#include "stdafx.h"
#include "PlayerIdle.h"
#include "PlayerWalk.h"
#include "PlayerAtk.h"
#include "PlayerDash.h"

CState* CPlayerIdle::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		return new CPlayerIdle(*this);

	case STATE_TYPE::WALK:
		pState = new CPlayerWalk(eDirection);
		break;

	case STATE_TYPE::ATK:
		pState = new CPlayerAtk();
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
