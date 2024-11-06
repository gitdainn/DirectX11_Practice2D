#include "stdafx.h"
#include "Enemy.h"
#include "Line_Manager.h"
#include "FileLoader.h"
#include "UI_Handler.h"
#include "Widget.h"
#include "EnemyJump.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy& rhs)
	: CSpriteObject(rhs)
	, m_StateFunctionMap(rhs.m_StateFunctionMap)
	, m_StateFunc(rhs.m_StateFunc)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Set_Layer(LAYER_ENEMY, false);
	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	//m_StateFunctionMap.emplace(ENEMY_STATE::IDLE, bind(&CEnemy::Idle, this, std::placeholders::_1)); // 뒤에 숫자면 되는데 placeholders면 안됨
	m_StateFunctionMap.emplace(ENEMY_STATE::IDLE, &CEnemy::Idle);
	m_StateFunctionMap.emplace(ENEMY_STATE::WALK, &CEnemy::Walk);
	m_StateFunctionMap.emplace(ENEMY_STATE::ATK1, &CEnemy::Attack);
	m_StateFunctionMap.emplace(ENEMY_STATE::CHASE, &CEnemy::Chase);
	m_StateFunctionMap.emplace(ENEMY_STATE::DAMAGED, &CEnemy::Damaged);
	m_StateFunctionMap.emplace(ENEMY_STATE::JUMP, &CEnemy::Jump);
	m_StateFunc = m_StateFunctionMap.find(ENEMY_STATE::IDLE)->second;

	return S_OK;
}

HRESULT CEnemy::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	// 흠.. 먼저 HasPossableLine 하는 게 좋을 듯하긴함.
	if (FAILED(Landing_Ground()))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tTransDesc;
	tTransDesc.SpeedPerSec = m_fMovementSpeed;
	m_pTransformCom->Set_TransformDesc(tTransDesc);

	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Landing_Ground()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	_float2 vPosition = _float2(0.f, 0.f);
	XMStoreFloat2(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (FAILED(pGameInstance->Get_CurrentLineEndPoint(vPosition, m_LineEndPoints)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	CTransform::TRANSFORM_DESC tTransDesc;
	tTransDesc.SpeedPerSec = m_fMovementSpeed;
	m_pTransformCom->Set_TransformDesc(tTransDesc);

	return S_OK;
}

_uint CEnemy::Tick(_double TimeDelta)
{
	if (0 >= m_iHp)
	{
		m_bIsDead = true;
	}

	if (nullptr != m_pWidgetCom)
	{
		m_pWidgetCom->Tick_BindToOwner(TimeDelta);
	}

	m_StateFunc(*this, TimeDelta);

	return __super::Tick(TimeDelta);
}

_uint CEnemy::LateTick(_double TimeDelta)
{
	Attach_Collider(m_pLayerTag, m_pColliderCom);

	return __super::LateTick(TimeDelta);
}

HRESULT CEnemy::Render()
{
	return __super::Render();
}

void CEnemy::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(pTarget);
	if (nullptr == pObject || nullptr == pTargetCollider)
		return;

	if (!lstrcmp(LAYER_PLAYERATK, pTargetLayer))
	{
		m_eTargetDirection = pObject->Get_SpriteDirection();
		Input_Handler(ENEMY_STATE::DAMAGED);
		Set_Damaged(pObject->Get_Attack());
		CUI_Handler::GetInstance()->Set_Hp(m_pWidgetCom, m_iHp);
	}

	__super::OnCollisionEnter(pTargetCollider, pTarget, pTargetLayer);
}

void CEnemy::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(pTarget);
	if (nullptr == pObject || nullptr == pTargetCollider)
		return;

	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
	if (!lstrcmp(LAYER_PLAYERATK, pTargetLayer))
	{
		int i = 0;
	}
}

void CEnemy::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	int i = 0;
}

#pragma region ENEMY_STATE
void CEnemy::Idle(_double TimeDelta)
{
}

void CEnemy::Walk(_double TimeDelta)
{
}
void CEnemy::Attack(_double TimeDelta)
{
}
void CEnemy::Chase(_double TimeDelta)
{
}
void CEnemy::Damaged(_double TimeDelta)
{
	// 흠.. InputHandler를 통해 Damaged가 실행될 때 진입점 함수(Enter)를 만들어서 한 번만 초기화해야 하는 것에 대해서 하기 ??
	// 이게 FSM을 이용한건가? 상태에 따라서 실행하는 함수/클래스를 달리하는 것 ... 이 교체 시점일 때 Enter를 호출하든.. 뭐 초기화를 하든!?
	m_DamagedTimeAcc += TimeDelta * 2.f;

	if (1.f <= m_DamagedTimeAcc)
	{
		m_DamagedTimeAcc = 0.0;

		if (FAILED(Landing_Ground()))
			Input_Handler(ENEMY_STATE::JUMP);
		else
			Input_Handler(ENEMY_STATE::IDLE);

		return;
	}

	// 넉백
	_vector vLerpPos = XMVectorLerp(m_vStartPosition, m_vDirectionPosition, CUtil::GetT_FastToSlow(m_DamagedTimeAcc));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);

	return;
}
void CEnemy::Jump(_double TimeDelta)
{
	if (nullptr == m_pEnemyJump)
	{
		MSG_BOX("CEnemy - Jump() - NULL");
		Input_Handler(ENEMY_STATE::IDLE);
	}

	if (m_pEnemyJump->Get_IsDead())
	{
		Safe_Delete(m_pEnemyJump);
		Input_Handler(ENEMY_STATE::IDLE);
	}

	m_pEnemyJump->Update(this, TimeDelta);
}
#pragma endregion

void CEnemy::Input_Handler(const ENEMY_STATE eEnemyState)
{
	if (m_StateFunctionMap.empty())
	{
		MSG_BOX("CEnemy - Input_Handler - Empty");
		return;
	}

	m_iAnimType = (_uint)eEnemyState;
	const ANIM_INFO* pAnim = m_pAnimInfo + m_iAnimType;
	m_iUVTextureIndex = pAnim->iStartIndex;

	auto iter = m_StateFunctionMap.find(eEnemyState);
	if (iter == m_StateFunctionMap.end())
		return;

	m_StateFunc = iter->second;

	Enter_State(eEnemyState);

	return;
}

void CEnemy::Execute_Skill(_uint iSkillIndex)
{
}

void CEnemy::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

void CEnemy::Enter_State(const ENEMY_STATE eEnemyState)
{
	// 플레이어 위치에 따라서
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	CTransform* pPlayerTransform = Get_PlayerTransformCom();
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPosition = vPosition - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float vDirectionVecX = XMVectorGetX(vPlayerPosition - vPosition);
	if (SPRITE_DIRECTION::LEFT == m_eSpriteDirection && 0.f > vDirectionVecX
		|| SPRITE_DIRECTION::RIGHT == m_eSpriteDirection && 0.f < vDirectionVecX)
	{
		Switch_SpriteDirection();
	}

	if (ENEMY_STATE::DAMAGED == eEnemyState)
	{
		_float	fKnockBackDistance = { 10.f };
		fKnockBackDistance *= m_eTargetDirection == SPRITE_DIRECTION::LEFT ? -1.f : 1.f;

		// 같은 곳을 바라보고 있다면 플레이어를 쳐다보게 방향 바꿈
		if (m_eSpriteDirection == m_eTargetDirection)
		{
			Switch_SpriteDirection();
		}

		m_vStartPosition = vPosition;
		m_vDirectionPosition = XMVectorSetX(m_vStartPosition, XMVectorGetX(m_vStartPosition) + fKnockBackDistance);
	}
	else if (ENEMY_STATE::JUMP == eEnemyState)
	{
		if (nullptr != m_pEnemyJump)
			Safe_Delete(m_pEnemyJump);

		m_pEnemyJump = CEnemyJump::Create();
		m_pEnemyJump->Set_IsFalling(true);
	}

	return;
}

/** @error - 현재 선이 바뀌는 시점 한 번씩만 Landing_Ground를 호출하고, 좌우로만 움직이고 있어 대각선으로 가려면 수정해야합니다. */
HRESULT CEnemy::Landing_Ground()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float2 fPosition = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
	_float fLandingY = { 0.f };

	if (pGameInstance->IsCurrentLineOccupied(fPosition, fLandingY))
	{
		// 선 양 끝 다시 셋팅
		if (FAILED(pGameInstance->Get_CurrentLineEndPoint(fPosition, m_LineEndPoints)))
		{
			Safe_Release(pGameInstance);
			MSG_BOX("CEnemy - Landing_Ground() - FAILED");
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
	}
	else
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CEnemy::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_Shader */
	CWidget::WIDGET_ARGUMENT tWidgetArgument;
	tWidgetArgument.pOwner = this;
	tWidgetArgument.pFilePath = TEXT("HealthBarW");
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_HealthBarWidget"),
		TAG_WIDGET, (CComponent**)&m_pWidgetCom, &tWidgetArgument)))
		return E_FAIL;

	///* For.Com_Shader */
	//CWidget::WIDGET_ARGUMENT tWidgetArgument;
	//tWidgetArgument.pOwner = this;
	//tWidgetArgument.pFilePath = TEXT("HealthBarW");
	//if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Widget"),
	//	TAG_WIDGET, (CComponent**)&m_pWidgetCom, &tWidgetArgument)))
	//	return E_FAIL;

#pragma region COLLIDER
	if (nullptr == m_pColliderCom)
	{
		COMPONENT_INFO tComponentInfo;
		ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
		tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
		tComponentInfo.fSize = _float2(70.f, 70.f);
		tComponentInfo.fOffset = _float2(0.f, tComponentInfo.fSize.y * 0.5f);

		if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TAG_COLL_AABB, (CComponent**)&m_pColliderCom, &tComponentInfo)))
		{
			MSG_BOX("CSpriteObject - Add_Components() - FAILED");
			return E_FAIL;
		}
		m_pColliderCom->Set_Owner(this);
	}

	COMPONENT_INFO tComponentInfo;
	ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
	tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
	tComponentInfo.fSize = _float2(70.f, 70.f);
	tComponentInfo.fOffset = _float2(0.f, tComponentInfo.fSize.y * 0.5f);

	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TAG_COLL_DEFAULTATK, (CComponent**)&m_pDefaultAtkColliderCom, &tComponentInfo)))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		return E_FAIL;
	}
	m_pDefaultAtkColliderCom->Set_Owner(this);
#pragma endregion

	return __super::Add_Components(pArg);
}

HRESULT CEnemy::SetUp_ShaderResources()
{
	if (FAILED(SetUp_ShaderDefault()))
		return E_FAIL;

	if (FAILED(SetUp_Shader_UVAnim()))
		return E_FAIL;

	return S_OK;
}

void CEnemy::Free()
{
	__super::Free();

	if (nullptr != m_pEnemyJump)
		Safe_Delete(m_pEnemyJump);

	Safe_Release(m_pDefaultAtkColliderCom);
	m_StateFunctionMap.clear();
}
