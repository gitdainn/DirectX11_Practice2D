#include "stdafx.h"
#include "Item.h"
#include "Line_Manager.h"
#include "FileLoader.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: CAnimObject(rhs)
{
}

HRESULT CItem::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	m_pLayerTag = LAYER_ITEM;
	m_eRenderGroup = CRenderer::RENDER_UI;

	return S_OK;
}

HRESULT CItem::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	if (FAILED(DefaultLineRider(m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
	{
		return E_FAIL;
	}
	m_bIsJump = true;

	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(DefaultLineRider(m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
	{
		return E_FAIL;
	}
	m_bIsJump = true;

	return S_OK;
}

HRESULT CItem::Late_Initialize()
{
	__super::Late_Initialize();

	if (FAILED(DefaultLineRider(m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
	{
		return E_FAIL;
	}
	m_bIsJump = true;

	return S_OK;
}

_uint CItem::Tick(_double TimeDelta)
{
	if (m_bIsJump)
	{
		Parabola(TimeDelta);

		if (m_bIsFalling)
		{
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fLandingY = { 0.f };
			if (m_pLineRiderCom->CheckLineLanding(vPosition, fLandingY))
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY + 20.f));
				m_bIsJump = false;
			}
		}
	}
	else
	{
		// 위아래로 왔다갔다하기
	}

	return __super::Tick(TimeDelta);
}

_uint CItem::LateTick(_double TimeDelta)
{
	Attach_Collider(m_pLayerTag, m_pColliderCom);

	return __super::LateTick(TimeDelta);
}

HRESULT CItem::Render()
{
	return __super::Render();
}

void CItem::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	__super::OnCollisionEnter(pTargetCollider, pTarget, pTargetLayer);
}

void CItem::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
}

void CItem::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
}

void CItem::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

HRESULT CItem::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_LineRider */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_LineRider"),
		TAG_LINERIDER, (CComponent**)&m_pLineRiderCom, &m_tSpriteInfo.fPosition)))
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
			MSG_BOX("CAnimObject - Add_Components() - FAILED");
			return E_FAIL;
		}
		m_pColliderCom->Set_Owner(this);
	}
#pragma endregion

	return __super::Add_Components(pArg);
}

HRESULT CItem::SetUp_ShaderResources()
{
	if (FAILED(SetUp_Shader_Camera()))
		return E_FAIL;

	return S_OK;
}

void CItem::Free()
{
	__super::Free();
}
