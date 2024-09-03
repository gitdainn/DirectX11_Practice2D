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
	
	m_pTransformCom->Set_Scaled(_float3(g_iWinSizeX, g_iWinSizeY, 1.f));

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(g_iWinSizeX, g_iWinSizeY, 1.f));

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	return S_OK;
}

_uint CBackGround::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CBackGround::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CBackGround::Render()
{
	return __super::Render();
}

HRESULT CBackGround::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Add_Animation()
{
	return;
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

CSpriteObject * CBackGround::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CBackGround::Clone(void* pArg) const
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
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
