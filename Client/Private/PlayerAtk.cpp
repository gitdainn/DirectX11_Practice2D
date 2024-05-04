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

	case STATE_TYPE::ATK1:
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
	pObject->Change_Sprite(STATE_TYPE::ATK1);
}

void CPlayerAtk::Update(CSpriteObject* pObject, _double TimeDelta)
{
	// 만약 애니메이션 끝났으면
	if (pObject->IsEndSprite())
	{
		pObject->Input_Handler(STATE_TYPE::IDLE);
	}
}
