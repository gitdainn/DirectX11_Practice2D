#include "stdafx.h"
#include "..\Public\Effect_Blue.h"

#include "GameInstance.h"

CEffect_Blue::CEffect_Blue(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CEffect_Blue::CEffect_Blue(const CEffect_Blue & rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffect_Blue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Blue::Initialize(void * pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	
	
	return S_OK;
}

_uint CEffect_Blue::Tick(_double TimeDelta)
{
	m_pVIBufferCom->Update(TimeDelta);

	return _uint();
}

_uint CEffect_Blue::LateTick(_double TimeDelta)
{

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return _uint();
}

HRESULT CEffect_Blue::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();		

	return S_OK;
}

HRESULT CEffect_Blue::Add_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc = { 10.f, XMConvertToRadians(90.0f) };
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom, nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC		BufferDesc;

	BufferDesc.vPosition = _float3(0.f, 10.f, 0.f);
	BufferDesc.vSize = _float2(200.f, 200.f);
	BufferDesc.fLifeTime = 5.f;
	BufferDesc.fMinSpeed = 3.f;
	BufferDesc.fMaxSpeed = 10.f;

	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &BufferDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;	

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CEffect_Blue::SetUp_ShaderResources()
{
	if (FAILED(m_pTransformCom->Set_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix",
		&pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix",
		&pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CEffect_Blue * CEffect_Blue::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Blue*		pInstance = new CEffect_Blue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEffect_Blue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Blue::Clone(void * pArg)
{
	CEffect_Blue*		pInstance = new CEffect_Blue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEffect_Blue");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Blue::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
