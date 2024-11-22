#include "stdafx.h"
#include "..\Public\HealthBarUI.h"
#include "GameInstance.h"

CHealthBarUI::CHealthBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CHealthBarUI::CHealthBarUI(const CHealthBarUI& rhs)
	: CUI(rhs)
{

}

HRESULT CHealthBarUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Set_Layer(LAYER_UI, false);

	return S_OK;
}

HRESULT CHealthBarUI::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Discard;
	m_eRenderGroup = CRenderer::RENDER_UI;

	return S_OK;
}

HRESULT CHealthBarUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Discard;
	m_eRenderGroup = CRenderer::RENDER_UI;

	return S_OK;
}

_uint CHealthBarUI::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CHealthBarUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CHealthBarUI::Render()
{
	return __super::Render();
}

HRESULT CHealthBarUI::Add_Components(void* pArg)
{
	return __super::Add_Components(pArg);
}

HRESULT CHealthBarUI::SetUp_ShaderResources()
{
	if (FAILED(SetUp_Shader_Orthographic()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fNormalizedX", &m_fNormalizedHp, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHealthBarUI* CHealthBarUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHealthBarUI* pInstance = new CHealthBarUI(pDevice, pContext);
	if (pInstance)
	{
		if (FAILED(pInstance->Initialize_Prototype()))
		{
			MSG_BOX("CHealthBarUI - Create() - FAILED");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CSpriteObject* CHealthBarUI::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CHealthBarUI* pInstance = new CHealthBarUI(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CHealthBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CHealthBarUI::Clone(void* pArg) const
{
	CHealthBarUI* pInstance = new CHealthBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHealthBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHealthBarUI::Free()
{
	__super::Free();
}
