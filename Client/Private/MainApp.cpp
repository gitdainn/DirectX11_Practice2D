#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InputHandler.h"

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

#ifdef _DEBUG
#include <dxgidebug.h>

#pragma comment(lib, "dxguid.lib")

void list_remaining_d3d_objects()
{
	HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

	IDXGIDebug* debug;

	GetDebugInterface(IID_PPV_ARGS(&debug));

	OutputDebugStringW(L"Starting Live Direct3D Object Dump:\r\n");
	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	OutputDebugStringW(L"Completed Live Direct3D Object Dump.\r\n");

	debug->Release();
}
#endif

void CMainApp::Tick(_double TimeDelta)
{
	/* 1. 현재 할당된 레벨의 Tick함수를 호출한다. */
	/* 2. 생성된 게임오브젝트의 Tick함수를 호출한다. */
	m_pGameInstance->Tick_Engine(TimeDelta);

	/** @qurious - 넘겨받는 커맨드 종류에 따라 UI를 넘길지 Player를 넘길지 어케 결정하지..? */
	/** @qurious - 왜 const CCommand면 -> Execute가 불가지? */
	CCommand* pCommand = CInputHandler::GetInstance()->Key_Input();
	if (nullptr != pCommand)
	{
		pCommand->Execute();
	}
	// Move면 플레이어 넘기고 .. 메뉴면 UI 넘기고 .. 
	// 근데 InputHandler에서 꼭 커맨드 안넘기고 걍 얘 cpp 안에서 메뉴 GetInstance해서 호출해도되나?
	// 근데 사실 커맨드 자체는 플레이어 말고도 몬스터도 특정 조건에 따라서 발동함.. 이건 걍 하면될듯?
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/no_0.dds")))))
		return E_FAIL;

	// CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/no_0.dds"));
	/* For.Prototype_Component_Shader_VtxNorTex*/
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;*/

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

	//Safe_Release(m_pRenderer); // Renderer는 컴포넌트에 추가되어 있어, 추후 Component에서 모두 Release하기 떄문에 X
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();

	//해제안된 COM객체 추적
#ifdef _DEBUG
	list_remaining_d3d_objects();
#endif
}

