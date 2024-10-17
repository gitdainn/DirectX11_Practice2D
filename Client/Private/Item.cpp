#include "stdafx.h"
#include "Item.h"
#include "Line_Manager.h"
#include "FileLoader.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CSpriteObject(rhs)
{
}

HRESULT CItem::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	m_LayerBitset = LAYER::ITEM;
	m_eRenderGroup = CRenderer::RENDER_UI;

	return S_OK;
}

HRESULT CItem::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Landing_Ground();

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Landing_Ground();

	return S_OK;
}

_uint CItem::Tick(_double TimeDelta)
{

	return __super::Tick(TimeDelta);
}

_uint CItem::LateTick(_double TimeDelta)
{
	Attach_Collider(m_LayerBitset, m_pColliderCom);

	return __super::LateTick(TimeDelta);
}

HRESULT CItem::Render()
{
	return __super::Render();
}

void CItem::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
	__super::OnCollisionEnter(pTargetCollider, pTarget);
}

void CItem::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
	__super::OnCollisionStay(pTargetCollider, pTarget);
}

void CItem::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CItem::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

void CItem::Landing_Ground()
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

HRESULT CItem::Add_Components(void* pArg)
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
#pragma endregion

	return __super::Add_Components(pArg);
}

HRESULT CItem::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CItem::Free()
{
	__super::Free();
}
