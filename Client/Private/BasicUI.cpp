#include "stdafx.h"
#include "..\Public\BasicUI.h"

#include "GameInstance.h"

CBasicUI::CBasicUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CBasicUI::CBasicUI(const CBasicUI& rhs)
	: CUI(rhs)
{

}

HRESULT CBasicUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_LayerBitset = LAYER::UI;

	return S_OK;
}

HRESULT CBasicUI::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

HRESULT CBasicUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

_uint CBasicUI::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CBasicUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CBasicUI::Render()
{
	return __super::Render();
}

HRESULT CBasicUI::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CBasicUI::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CBasicUI* CBasicUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBasicUI* pInstance = new CBasicUI(pDevice, pContext);
	if (pInstance)
	{
		if (FAILED(pInstance->Initialize()))
		{
			MSG_BOX("CBasicUI - Create() - FAILED");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CSpriteObject* CBasicUI::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CBasicUI* pInstance = new CBasicUI(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CBasicUI::Clone(void* pArg) const
{
	CBasicUI* pInstance = new CBasicUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBasicUI::Free()
{
	__super::Free();
}
