#include "stdafx.h"
#include "..\Public\Environment.h"

#include "GameInstance.h"

CEnvironment::CEnvironment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{

}

CEnvironment::CEnvironment(const CEnvironment& rhs)
	: CSpriteObject(rhs)
{

}

HRESULT CEnvironment::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Set_Layer(LAYER_ENVIRONMENT, false);

	return S_OK;
}

HRESULT CEnvironment::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;

	if (!lstrcmp(L"GateWall", m_pNameTag))
		m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return S_OK;
}

HRESULT CEnvironment::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;

	if (!lstrcmp(L"GateWall", m_pNameTag))
		m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return S_OK;
}

_uint CEnvironment::Tick(_double TimeDelta)
{
	m_iOrder += 10;
	return __super::Tick(TimeDelta);
}

_uint CEnvironment::LateTick(_double TimeDelta)
{
	//if(nullptr != m_pColliderCom)
	//	m_pColliderCom->Set_IsBlock(true);

	if (nullptr != m_pColliderCom)
		Attach_Collider(m_pLayerTag, m_pColliderCom);

	return __super::LateTick(TimeDelta);
}

HRESULT CEnvironment::Render()
{
	return __super::Render();
}

HRESULT CEnvironment::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CEnvironment::SetUp_ShaderResources()
{
	if (FAILED(SetUp_Shader_Camera()))
		return E_FAIL;

	return S_OK;
}

void CEnvironment::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget || nullptr == pTargetLayer)
		return;

	if (!lstrcmp(L"GateWall", m_pNameTag))
	{
		// 스테이지 매니저에서 Clear일 때
		// 이펙트 생성
		// 아이템 생성

		// 흠.. 이때 획득한 아이템을 먹으면 스테이지 내 문에게 알림을 걸어서 ACTIVATE로 활성화
		// 나는 현재 그냥 바로 문들 활성화 시키기

	}
	__super::OnCollisionEnter(pTargetCollider, pTarget, pTargetLayer);
}

void CEnvironment::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget || nullptr == pTargetLayer)
		return;

	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
}

void CEnvironment::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget || nullptr == pTargetLayer)
		return;

	__super::OnCollisionExit(pTargetCollider, pTarget, pTargetLayer);
}

CEnvironment* CEnvironment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnvironment* pInstance = new CEnvironment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEnvironment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CEnvironment::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CEnvironment* pInstance = new CEnvironment(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CEnvironment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CEnvironment::Clone(void* pArg) const
{
	CEnvironment* pInstance = new CEnvironment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnvironment");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnvironment::Free()
{
	__super::Free();
}
