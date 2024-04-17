#include "..\Public\Renderer.h"
#include "BlendObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"

#include "Shader.h"
#include "PipeLine.h"
#include "VIBuffer_Rect.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);

}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, 
		TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, 
		TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, 
		TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext,
		TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;
	
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
#endif // _DEBUG




	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (RENDER_END <= eRenderGroup || 0 > (_uint)eRenderGroup
		|| nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroups[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRenderGroup(CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	m_DebugGroup.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{

	if (FAILED(Draw_Priority()))
		return E_FAIL;

	if (FAILED(Draw_NonBlend()))
		return E_FAIL;

	if (FAILED(Draw_LightAcc()))
		return E_FAIL;

	if (FAILED(Draw_DeferredBlend()))
		return E_FAIL;

	if (FAILED(Draw_NonLight()))
		return E_FAIL;

	if (FAILED(Draw_Blend()))
		return E_FAIL;

	if (FAILED(Draw_UI()))
		return E_FAIL;



#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Draw_Priority()
{	
	for (auto& pGameObject : m_RenderGroups[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Draw_NonBlend()
{
	/* Diffuse + Normal */
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"));
	

	/* Diffuse + Normal 타겟에 그리는 작업을 수행한다. */
	for (auto& pGameObject : m_RenderGroups[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_NONBLEND].clear();

	m_pTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::Draw_NonLight()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Draw_Blend()
{	
	m_RenderGroups[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
	});

	for (auto& pGameObject : m_RenderGroups[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Draw_UI()
{
	for (auto& pGameObject : m_RenderGroups[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Draw_LightAcc()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	/* Shade 타겟에 그리는 작업을 수행한다. */
	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CPipeLine*		pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);
	

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	/* 따로 빼놨던 백버퍼를 장치에 복구한다. */
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Draw_DeferredBlend()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
		
	/* 백버퍼에 그린다. */
	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Set_ShaderResourceView(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Render();

	
	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pDebugCom : m_DebugGroup)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugGroup.clear();

	m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pTarget_Manager->Render_MRT(TEXT("MRT_Deferred"), m_pShader, m_pVIBuffer);
	m_pTarget_Manager->Render_MRT(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);

	return S_OK;
}
#endif // _DEBUG

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer *	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{	
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroups[i])
			Safe_Release(pGameObject);

		m_RenderGroups[i].clear();
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	

}
