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
		return E_FAIL;

	if(FAILED(Add_Components()))
		return E_FAIL;

	m_iShaderPassIndex = (int)VTXTEXPASS::Default;
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

	m_pShaderCom->Begin(m_iShaderPassIndex);

	m_pVIBufferCom->Render();		

	return S_OK;
}

HRESULT CBackGround::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;	

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
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
}
