//#include "stdafx.h"
//#include "ToolApp.h"
//#include "GameInstance.h"
//
//USING(Tool)
//
//CToolApp::CToolApp()
//	: m_pGameInstance(CGameInstance::GetInstance())
//{
//	Safe_AddRef(m_pGameInstance);
//}
//
//HRESULT CToolApp::Initialize()
//{
//	GRAPHIC_DESC			GraphicDesc;
//	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);
//
//	GraphicDesc.eWinMode = GRAPHIC_DESC::MODE_WIN;
//	GraphicDesc.hWnd = g_hWnd;
//	GraphicDesc.iViewSizeX = g_iWinSizeX;
//	GraphicDesc.iViewSizeY = g_iWinSizeY;
//
//	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
//		return E_FAIL;
//
//	/*
//	MakeSpriteFont "배찌체" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 133ex.spritefont
//	*/
//	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bazzi"), TEXT("../Bin/Resources/Fonts/133ex.SpriteFont"))))
//		return E_FAIL;
//
//	if (FAILED(Ready_Prototype_Component_For_Static()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Prototype_GameObject_For_Static()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Prototype_Sprite_For_Static()))
//		return E_FAIL;
//
//	if (FAILED(SetUp_StartLevel(LEVEL_LOGO)))
//		return E_FAIL;
//
//
//	return S_OK;
//}
//
//#ifdef _DEBUG
//#include <dxgidebug.h>
//
//#pragma comment(lib, "dxguid.lib")
//
//void list_remaining_d3d_objects()
//{
//	HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
//	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));
//
//	IDXGIDebug* debug;
//
//	GetDebugInterface(IID_PPV_ARGS(&debug));
//
//	OutputDebugStringW(L"Starting Live Direct3D Object Dump:\r\n");
//	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
//	OutputDebugStringW(L"Completed Live Direct3D Object Dump.\r\n");
//
//	debug->Release();
//}
//#endif
//
//void CToolApp::Tick(_double TimeDelta)
//{
//	/** @qurious - 넘겨받는 커맨드 종류에 따라 UI를 넘길지 Player를 넘길지 어케 결정하지..? */
//	/** @qurious - 왜 const CCommand면 -> Execute가 불가지? */
//	/** @note - 매개변수가 이중포인터일 때 CGameObject* pObject를 &pObject로 넘기면 값 못 받아옴. */
//	/** @note - CGameObject** pObject로 이중포인터일 경우 nullptr인데 *pObject처럼 값에 접근하면 오류 뜸! */
//	//CGameObject** pObject = nullptr;
//	//CCommand* pCommand = CInputHandler::GetInstance()->Key_Input(pObject);
//
//	/* 1. 현재 할당된 레벨의 Tick함수를 호출한다. */
//	/* 2. 생성된 게임오브젝트의 Tick함수를 호출한다. */
//	m_pGameInstance->Tick_Engine(TimeDelta);
//
//
//#ifdef _DEBUG
//	m_TimeAcc += TimeDelta;
//#endif // _DEBUG
//}
//
//HRESULT CToolApp::Render()
//{
//	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
//	m_pGameInstance->Clear_DepthStencil_View();
//
//	m_pRenderer->Draw_RenderGroup();
//
//	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Bazzi"), TEXT("Dainn"), _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
//		return E_FAIL;
//
//#ifdef _DEBUG
//	++m_dwNumDraw;
//
//	if (m_TimeAcc >= 1.0)
//	{
//		wsprintf(m_szFPS, TEXT("fps : %d"), m_dwNumDraw);
//		m_TimeAcc = 0.0;
//		m_dwNumDraw = 0;
//	}
//
//	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Bazzi"), m_szFPS, _float2(0.f, 35.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
//		return E_FAIL;
//#endif
//
//	m_pGameInstance->Present();
//
//	return S_OK;
//}
//
//HRESULT CToolApp::SetUp_StartLevel(LEVEL eNextLevelID)
//{
//	//return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevelID));
//}
//
//HRESULT CToolApp::Ready_Prototype_Component_For_Static()
//{
//	///* For.Prototype_Component_Renderer */
//	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
//	//	m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
//	//	return E_FAIL;
//
//	///* For.Prototype_Component_Transform*/
//	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
//	//	CTransform::Create(m_pDevice, m_pContext))))
//	//	return E_FAIL;
//
//	///* For.Prototype_Component_VIBuffer_Rect */
//	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
//	//	CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
//	//	return E_FAIL;
//
//	///* For.Prototype_Component_Shader_VtxTex*/
//	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
//	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
//	//	return E_FAIL;
//
//	return S_OK;
//}
//
//HRESULT CToolApp::Ready_Prototype_GameObject_For_Static()
//{
//	return S_OK;
//}
//
//HRESULT CToolApp::Ready_Prototype_Sprite_For_Static()
//{
//	return S_OK;
//}
//
//CToolApp* CToolApp::Create()
//{
//	CToolApp* pInstance = new CToolApp();
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MSG_BOX("Failed to Created CToolApp");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CToolApp::Free()
//{
//	__super::Free();
//
//	//Safe_Release(m_pRenderer); // Renderer는 컴포넌트에 추가되어 있어, 추후 Component에서 모두 Release하기 떄문에 X
//	Safe_Release(m_pContext);
//	Safe_Release(m_pDevice);
//	Safe_Release(m_pGameInstance);
//
//	/** @note - 싱글톤도 new로 동적할당했기에 꼭 직접 해제를 명시해줘야함 (delete 해줘야 한다는 뜻) */
//	CGameInstance::Release_Engine();
//
//	//해제안된 COM객체 추적
//#ifdef _DEBUG
//	list_remaining_d3d_objects();
//#endif
//}
//