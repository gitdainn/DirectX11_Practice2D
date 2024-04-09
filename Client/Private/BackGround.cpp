#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSpriteObject(pDevice, pContext)
{

}

CBackGround::CBackGround(const CBackGround & rhs)
	: CSpriteObject(rhs)
{

}

HRESULT CBackGround::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CBackGround::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{

	}

	return S_OK;
}

_uint CBackGround::Tick(_double TimeDelta)
{
	return _uint();
}

_uint CBackGround::LateTick(_double TimeDelta)
{

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	return _uint();
}

HRESULT CBackGround::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();		

	return S_OK;
}

HRESULT CBackGround::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;	

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture "), (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject * CBackGround::Clone(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
