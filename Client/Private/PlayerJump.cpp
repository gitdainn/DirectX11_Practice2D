#include "stdafx.h"
#include "PlayerJump.h"
#include "PlayerIdle.h"

CPlayerJump::CPlayerJump()
{
}

CPlayerJump::~CPlayerJump()
{
}

CState* CPlayerJump::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		pState = new CPlayerIdle();
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerJump::Enter(CSpriteObject* pObject)
{
	pObject->Change_Sprite(STATE_TYPE::DASH);
}

void CPlayerJump::Update(CSpriteObject* pObject, _double TimeDelta)
{
	// 만약 애니메이션 끝났으면
	if (pObject->IsEndSprite())
	{
		pObject->Input_Handler(STATE_TYPE::IDLE);
	}
	else
	{
		pObject->Get_TransformCom()->Go_Left(TimeDelta * 10);
	}
	/*m_eDirection == LOOK_DIRECTION::LEFT ?
		pObject->Get_TransformCom()->Go_Left(TimeDelta * 200) : pObject->Get_TransformCom()->Go_Right(TimeDelta * 200);*/
}
