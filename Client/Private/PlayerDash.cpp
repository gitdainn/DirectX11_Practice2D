#include "stdafx.h"
#include "PlayerDash.h"
#include "PlayerIdle.h"
#include "PlayerAtk.h"

CPlayerDash::CPlayerDash(const SPRITE_DIRECTION eDirection)
	: m_eDirection(eDirection)
{
}

CPlayerDash::~CPlayerDash()
{
}

CState* CPlayerDash::Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	switch (Input)
	{
	case STATE_TYPE::IDLE:
		pState = new CPlayerIdle();
		break;

	case STATE_TYPE::ATK:
		pState = new CPlayerAtk();
		break;

	case STATE_TYPE::DASH:
		pState = new CPlayerDash(*this);
		break;

	default:
		break;
	}

	return pState;
}

void CPlayerDash::Enter(CSpriteObject* pObject)
{
	pObject->Change_Sprite(STATE_TYPE::DASH);

	if (SPRITE_DIRECTION::DIRECTION_END == m_eDirection)
	{
		m_eDirection = pObject->Get_SpriteDirection();
	}
}

void CPlayerDash::Update(CSpriteObject* pObject, _double TimeDelta)
{
	// 만약 애니메이션 끝났으면
	if (pObject->IsEndSprite())
	{
		pObject->Input_Handler(STATE_TYPE::IDLE);
	}
	else
	{
		const float fSpeed = 20.f;

		if (SPRITE_DIRECTION::LEFT == m_eDirection)
		{
			pObject->Get_TransformCom()->Go_Left(TimeDelta * fSpeed);
		}
		else if (SPRITE_DIRECTION::RIGHT == m_eDirection)
		{
			pObject->Get_TransformCom()->Go_Right(TimeDelta * fSpeed);
		}
	}
}
