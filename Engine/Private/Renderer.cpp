#include "..\Public\Renderer.h"
#include "BlendObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"

#include "Shader.h"
#include "PipeLine.h"
#include "VIBuffer_Rect.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);

}

HRESULT CRenderer::Initialize_Prototype()
{
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	//@ error - Shader_Deffered 없어서 오류 m_pTarget_Manager로 출력을 수행하는 구문은 현재 전부 오류 뜸
	//m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	//if (nullptr == m_pShader)
	//	return E_FAIL;

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

	if (FAILED(Draw_Blend()))
		return E_FAIL;

	if (FAILED(Draw_UI()))
		return E_FAIL;

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
	/* Diffuse + Normal 타겟에 그리는 작업을 수행한다. */
	for (auto& pGameObject : m_RenderGroups[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroups[RENDER_NONBLEND].clear();

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
	// 딴 컴포넌트들은 사본 줄 때 new로 만들기 때문에 AddRef 없이 해당 컴포넌트를 해지해줌.
	// 얘는 원본을 추가적으로 참조하는 거니까 AddRef() 해주고 해제해주기
	AddRef();
	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pLight_Manager);

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroups[i])
			Safe_Release(pGameObject);

		m_RenderGroups[i].clear();
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	

}
