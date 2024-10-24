#include "stdafx.h"
#include "..\Public\DisplayUI.h"

#include "GameInstance.h"

CDisplayUI::CDisplayUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CDisplayUI::CDisplayUI(const CDisplayUI& rhs)
	: CUI(rhs)
{

}

HRESULT CDisplayUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_pLayerTag = LAYER_UI;

	return S_OK;
}

HRESULT CDisplayUI::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

HRESULT CDisplayUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

_uint CDisplayUI::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CDisplayUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CDisplayUI::Render()
{
	return __super::Render();
}

HRESULT CDisplayUI::Add_Components(void* pArg)
{
	return __super::Add_Components(pArg);
}

HRESULT CDisplayUI::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CDisplayUI* CDisplayUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDisplayUI* pInstance = new CDisplayUI(pDevice, pContext);
	if (pInstance)
	{
		if (FAILED(pInstance->Initialize_Prototype()))
		{
			MSG_BOX("CDisplayUI - Create() - FAILED");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CSpriteObject* CDisplayUI::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CDisplayUI* pInstance = new CDisplayUI(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CDisplayUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CDisplayUI::Clone(void* pArg) const
{
	CDisplayUI* pInstance = new CDisplayUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDisplayUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDisplayUI::Free()
{
	__super::Free();
}
