#include "stdafx.h"
#include "Enemy.h"
#include "Line_Manager.h"
#include "FileLoader.h"

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
	m_StateFunc = m_StateFunctionMap.find(ENEMY_STATE::IDLE)->second;

	return S_OK;
}

HRESULT CEnemy::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Landing_Ground();

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

HRESULT CEnemy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Landing_Ground();

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

void CEnemy::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
	CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(pTarget);
	if (nullptr == pObject || nullptr == pTargetCollider)
		return;

	if (!lstrcmp(LAYER_PLAYERATK, pTarget->Get_Layer()))
	{
		Input_Handler(ENEMY_STATE::DAMAGED);
		Set_Damaged(pObject->Get_Attack());
	}

	__super::OnCollisionEnter(pTargetCollider, pTarget);
}

void CEnemy::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
	CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(pTarget);
	if (nullptr == pObject || nullptr == pTargetCollider)
		return;

	__super::OnCollisionStay(pTargetCollider, pTarget);
}

void CEnemy::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
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
}
#pragma endregion

void CEnemy::Input_Handler(const ENEMY_STATE eEnemyState)
{
	if (m_StateFunctionMap.empty())
	{
		MSG_BOX("CEnemy - Input_Handler - Empty");
		return;
	}

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

	m_iAnimType = (_uint)eEnemyState;
	const ANIM_INFO* pAnim = m_pAnimInfo + m_iAnimType;
	m_iUVTextureIndex = pAnim->iStartIndex;

	auto iter = m_StateFunctionMap.find(eEnemyState);
	if (iter == m_StateFunctionMap.end())
		return;

	m_StateFunc = iter->second;
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

void CEnemy::Landing_Ground()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fLandingY = { 0.f };
	if (pGameInstance->IsCurrentLineOccupied(_float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition)), fLandingY))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
	}

	Safe_Release(pGameInstance);

	return;
}

HRESULT CEnemy::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

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
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(m_pDefaultAtkColliderCom);
	m_StateFunctionMap.clear();
}
