#include "stdafx.h"
#include "..\Public\UI.h"

#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{

}

CUI::CUI(const CUI& rhs)
	: CSpriteObject(rhs)
	, m_pOwner(nullptr)
{

}

HRESULT CUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	m_pLayerTag = LAYER_UI;
	m_bIsScroll = false;

	return S_OK;
}

HRESULT CUI::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
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

void CUI::Free()
{
	__super::Free();
}
