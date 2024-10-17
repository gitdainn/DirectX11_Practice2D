#include "stdafx.h"
#include "..\Public\UI.h"

#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{

}

CUI::CUI(const CUI& rhs)
	: CSpriteObject(rhs)
{

}

HRESULT CUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_LayerBitset = LAYER::UI;

	return S_OK;
}

HRESULT CUI::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

_uint CUI::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CUI::Render()
{
	return __super::Render();
}

HRESULT CUI::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CUI::SetUp_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Set_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat();
	//if (m_bIsScroll)
	//{
	//	Scroll_Screen(WorldMatrix);
	//}

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
		return E_FAIL;

	if (nullptr == m_pSpriteFileName)
	{
		if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_pSpriteFileName)))
			return E_FAIL;
	}


	return S_OK;
}

void CUI::Free()
{
	__super::Free();
}
