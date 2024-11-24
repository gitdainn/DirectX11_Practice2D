#include "stdafx.h"
#include "PlayerWalk.h"
#include "PlayerAtk.h"
#include "PlayerIdle.h"
#include "PlayerDash.h"
#include "PlayerDamaged.h"

CPlayerWalk::CPlayerWalk(const SPRITE_DIRECTION eDir)
	:m_eDirection(eDir)
{
}

CPlayerWalk::~CPlayerWalk()
{
}

CState* CPlayerWalk::Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = { nullptr };

	/** @note - 이걸.. 보관해서 못 쓰는 이유가 상태마다 모든 상태 보유 시 겹치니까.. 비효율 */
	switch (Input)
	{
	case STATE_TYPE::WALK:
		pState = new CPlayerWalk(eDirection);
		break;

	case STATE_TYPE::IDLE:
		pState = new CPlayerIdle();
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

void CPlayerWalk::Enter(CPlayer* pObject)
{
	// if ATK이나 JUMP 등이 아니면 교체
	if (STATE_TYPE::IDLE == pObject->Get_CurrentState())
	{
		pObject->Set_CurrentState(STATE_TYPE::WALK);
		pObject->Change_AnimType((_uint)STATE_TYPE::WALK);
	}
		
	pObject->Set_SpriteDirection(m_eDirection);

	VTXTEX_PASS eShaderPass = VTXTEX_PASS::Default;
	if (SPRITE_DIRECTION::DIRECTION_END == m_eDirection)
	{
		MSG_BOX("CPlayerWalk - Enter() - Direction is END");
		return;
	}

	if (SPRITE_DIRECTION::LEFT == m_eDirection)
	{
		eShaderPass = VTXTEX_PASS::FlipUV_Anim;
	}
	else if (SPRITE_DIRECTION::RIGHT == m_eDirection)
	{
		eShaderPass = VTXTEX_PASS::UV_Anim;
	}

	pObject->Set_ShaderPass((_uint)eShaderPass);
}

void CPlayerWalk::Update(CPlayer* pObject, _double TimeDelta)
{
	if (nullptr == pObject)
		return;

	CTransform* pPlayerTransform = pObject->Get_TransformCom();
	if (nullptr == pPlayerTransform)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	const _float fSpeed = { 10.f };
	if (SPRITE_DIRECTION::LEFT == m_eDirection)
	{
		_vector vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		pObject->Get_TransformCom()->Go_Left(fSpeed * TimeDelta);
		_vector vAfterMovePosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(abs(XMVectorGetX(vPosition) - XMVectorGetX(vAfterMovePosition)), 0.f);
		//pGameInstance->Set_Scroll(fScroll);
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());

	}
	else if (SPRITE_DIRECTION::RIGHT == m_eDirection)
	{
		_vector vPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		pObject->Get_TransformCom()->Go_Right(fSpeed * TimeDelta);
		_vector vAfterMovePosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(-1 * abs(XMVectorGetX(vPosition) - XMVectorGetX(vAfterMovePosition)), 0.f);
		//pGameInstance->Set_Scroll(fScroll);
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());

	}

	Safe_Release(pGameInstance);
}
