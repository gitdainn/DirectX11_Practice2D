#include "stdafx.h"
#include "..\Public\SkillUI.h"

#include "GameInstance.h"

CSkillUI::CSkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{

}

CSkillUI::CSkillUI(const CSkillUI& rhs)
	: CUI(rhs)
{

}

HRESULT CSkillUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_pLayerTag = LAYER_UI;

	return S_OK;
}

HRESULT CSkillUI::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

HRESULT CSkillUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_UI;
	return S_OK;
}

_uint CSkillUI::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CSkillUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CSkillUI::Render()
{
	return __super::Render();
}

HRESULT CSkillUI::Add_Components(void* pArg)
{
	return __super::Add_Components(pArg);
}

HRESULT CSkillUI::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CSkillUI* CSkillUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillUI* pInstance = new CSkillUI(pDevice, pContext);
	if (pInstance)
	{
		if (FAILED(pInstance->Initialize_Prototype()))
		{
			MSG_BOX("CSkillUI - Create() - FAILED");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CSpriteObject* CSkillUI::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CSkillUI* pInstance = new CSkillUI(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillUI::Clone(void* pArg) const
{
	CSkillUI* pInstance = new CSkillUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillUI::Free()
{
	__super::Free();
}
