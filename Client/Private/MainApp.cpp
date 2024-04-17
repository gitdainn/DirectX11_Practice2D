#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{	
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::MODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewSizeX = g_iWinSizeX;
	GraphicDesc.iViewSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/*
	MakeSpriteFont "배찌체" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 133ex.spritefont
	*/ 
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bazzi"), TEXT("../Bin/Resources/Fonts/133ex.SpriteFont"))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Sprite_For_Static()))
		return E_FAIL;

	if (FAILED(SetUp_StartLevel(LEVEL_LOGO)))
		return E_FAIL;


	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	/* 1. 현재 할당된 레벨의 Tick함수를 호출한다. */
	/* 2. 생성된 게임오브젝트의 Tick함수를 호출한다. */
	m_pGameInstance->Tick_Engine(TimeDelta);


#ifdef _DEBUG
	m_TimeAcc += TimeDelta;
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Bazzi"), TEXT("다인잉"), _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	++m_dwNumDraw;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_dwNumDraw);
		m_TimeAcc = 0.0;
		m_dwNumDraw = 0;
	}

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Bazzi"), m_szFPS, _float2(0.f, 35.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(LEVEL eNextLevelID)
{		
	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevelID));	
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex*/
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;*/

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject_For_Static()
{
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Sprite_For_Static()
{
	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();
}

