#include "stdafx.h"
#include "PlayerAtk.h"
#include "PlayerIdle.h"

CState* CPlayerAtk::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		pState = new CPlayerIdle();
		break;

	case STATE_TYPE::DASH:
		break;

	default:
		break;
	}
	return pState;
}

void CPlayerAtk::Enter(CSpriteObject* pObject)
{
	// if ATK이나 JUMP 등이 아니면 교체
	pObject->Change_Sprite(STATE_TYPE::ATK);
}

void CPlayerAtk::Update(CSpriteObject* pObject, _double TimeDelta)
{
	// 만약 애니메이션 끝났으면
	if (pObject->IsEndSprite())
	{
		pObject->Input_Handler(STATE_TYPE::IDLE);
	}
}
