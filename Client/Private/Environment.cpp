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
	
	return S_OK;
}

HRESULT CEnvironment::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	return S_OK;
}

HRESULT CEnvironment::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	return S_OK;
}

_uint CEnvironment::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CEnvironment::LateTick(_double TimeDelta)
{
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
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CEnvironment::Add_Animation()
{
	return;
}

void CEnvironment::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CEnvironment::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CEnvironment::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
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

CSpriteObject* CEnvironment::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
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
