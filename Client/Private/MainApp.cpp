#include "stdafx.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InputHandler.h"
#include "Player_Manager.h"
#include "FileLoader.h"
#include "UI_Handler.h"
#include "Utility.h"
#include "Widget.h"
#include "HealthBarWidget.h"
#include "LineRider.h"
#include "Camera_Dynamic.h"
#include "Stage_Manager.h"

#pragma region LOAD
#include "ColliderAABB2D.h"
#include "SkillGateOfNether.h"
#include "SkillGuillotine.h"
//#include "SkillDive.h"
//#include "SkillHighTide.h"
//#include "SkillThrowSkul.h"
//#include "SkillHeadless.h"
#pragma endregion

USING(Client)

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
	MakeSpriteFont "ttf이름" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 133ex.spritefont
	*/

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Skul"), TEXT("../Bin/Resources/Fonts/Perfect DOS VGA 437.spritefont"))))
		return E_FAIL;

	if (FAILED(Ready_CollisionLayerMatrix()))
		return E_FAIL;

	if (FAILED(Ready_DefaultData_Excel()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
	{
		MSG_BOX("CMainApp - Initialize() - Ready_Prototype_Component_For_Static() - FAILED");
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_GameObject_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Sprite_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_PrototypeName()))
		return E_FAIL;

	if (FAILED(SetUp_StartLevel(LEVEL_LOGO)))
		return E_FAIL;

	CUI_Handler* pUIHandler = CUI_Handler::GetInstance();
	if (nullptr == pUIHandler)
		return E_FAIL;
	pUIHandler->Initialize();

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
	/** @qurious - 넘겨받는 커맨드 종류에 따라 UI를 넘길지 Player를 넘길지 어케 결정하지..? */
	/** @qurious - 왜 const CCommand면 -> Execute가 불가지? */
	/** @note - 매개변수가 이중포인터일 때 CGameObject* pObject를 &pObject로 넘기면 값 못 받아옴. */
	/** @note - CGameObject** pObject로 이중포인터일 경우 nullptr인데 *pObject처럼 값에 접근하면 오류 뜸! */
	//CGameObject** pObject = nullptr;
	//CCommand* pCommand = CInputHandler::GetInstance()->Key_Input(pObject);

	CInputHandler::GetInstance()->Key_Input();

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

#ifdef _DEBUG
	//@ note - 프레임 60(1초) 제한 코드
	m_pGameInstance->Render_Line();
	m_pGameInstance->Render_Collider();

	++m_dwNumDraw;

	if (m_TimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_dwNumDraw);
		m_TimeAcc = 0.0;
		m_dwNumDraw = 0;
	}

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Skul"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	int iCount = CStage_Manager::GetInstance()->Get_EnemyCount();
	_tchar		szCount[MAX_PATH] = TEXT("");
	wsprintf(szCount, TEXT("EnemyNum : %d"), iCount);
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Skul"), szCount, _float2(0.f, 70.f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#endif

	m_pGameInstance->Clear_Collider();
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(LEVEL eNextLevelID)
{		
	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevelID));	
}

HRESULT CMainApp::Ready_CollisionLayerMatrix()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	pGameInstance->Set_CollisionLayerMatrix(LAYER_PLAYER, LAYER_ENVIRONMENT, true);
	pGameInstance->Set_CollisionLayerMatrix(LAYER_PLAYER, LAYER_ENEMYATK, true);
	pGameInstance->Set_CollisionLayerMatrix(LAYER_PLAYER, LAYER_ITEM, true);
	pGameInstance->Set_CollisionLayerMatrix(LAYER_ENEMY,  LAYER_PLAYERATK, true);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CMainApp::Ready_DefaultData_Excel()
{
	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
		return E_FAIL;
	Safe_AddRef(pFileLoader);

	if (FAILED(pFileLoader->Load_SkulData_Excel(TEXT("../../Data.xlsx"))))
	{
		MSG_BOX("CMainApp - Ready_DefaultData_Excel() - FAILED");
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_SkillData_Excel(TEXT("../../Data.xlsx"))))
	{
		MSG_BOX("CMainApp - Ready_DefaultData_Excel() - FAILED");
		return E_FAIL;
	}

	Safe_Release(pFileLoader);
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Sprite_For_Static()
{
#pragma region PLAYER_TEXTURE	
	///* For.Prototype_Component_Texture_Logo */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Background"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Background/Forest_%d.png")))))
	//{
	//	Safe_Release(m_pGameInstance);
	//	return E_FAIL;
	//};

	/* For.Prototype_Component_Sprite_LittleBorn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_LittleBorn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/LittleBorn/Wait_%d.png"), 48))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_GrimReaper */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GrimReaperUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/GrimReaper/GrimReaperUV_%d.png")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_WaterSkul */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_WaterSkulUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/WaterSkul/WaterSkulUV_%d.png")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_GrimReaper */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/Bat.png")))))
	{
		return E_FAIL;
	};
#pragma endregion

#pragma region ENEMY_TEXTURE
	/* For.Prototype_Component_Sprite_GrimReaper */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Enemy_Solider"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Enemy/Enemy_Solider_B.png")))))
	{
		return E_FAIL;
	};
#pragma endregion

#pragma region EFFECT_TEXTURE	
	///* For.Prototype_Component_Sprite_Background */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GateOfNether"),
	//	CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/GrimReaper_Skill/GateOfNether/")))))
	//{
	//	return E_FAIL;
	//};

	/* For.Prototype_Component_Sprite_GateOfNetherUV */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GateOfNetherUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/GrimReaper_Skill/GateOfNether/GrimReaper_GateOfNetherUV_%d.png"), 2))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_Guillotine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Guillotine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/GrimReaper_Skill/Guillotine/GrimReaper_Guillotine_%d.png"), 26))))
	{
		return E_FAIL;
	};
	/* For.Prototype_Component_Sprite_GuillotineSign */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GuillotineSign"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Player/GrimReaper_Skill/Guillotine/GrimReaper_Guillotine_Sign_%d.png"), 10))))
	{
		return E_FAIL;
	};
#pragma endregion

#pragma region FOLDER
	/* For.Prototype_Component_Sprite_ForestTile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_ForestTile"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Tiles/ForestTile/")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_ForestEnvironment */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_ForestEnvironment"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Environment/Forest/")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Background"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Background/")))))
	{
		return E_FAIL;
	};
#pragma endregion

#pragma region ETC
	/* For.Prototype_Component_Sprite_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Door"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/Door/")))))
	{
		return E_FAIL;
	};
#pragma endregion

#pragma region UI
	/* For.Prototype_Component_Sprite_SkulUI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_UI_Skul"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/UI/Skul/")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_UI_HealthBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_UI_HealthBar"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/UI/HealthBar/")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_UI_Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_UI_Frame"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/UI/Frame/")))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_UI_Key */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_UI_Key"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sprite/UI/Key/")))))
	{
		return E_FAIL;
	};
#pragma endregion

	// CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/no_0.dds"));
	/* For.Prototype_Component_Shader_VtxNorTex*/
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;*/

	return S_OK;
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

	/* For.Prototype_Component_LineRider*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_LineRider"),
		CLineRider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Widget */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Widget"),
		CWidget::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	};

	/* For.Prototype_Component_HealthBarWidget */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_HealthBarWidget"),
		CHealthBarWidget::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	};

#pragma region COLLIDER
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CColliderAABB2D::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	};
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject_For_Static()
{
#pragma region OBJECT
	/* For.Prototype_GameObject_Install */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(m_pGameInstance);
		return E_FAIL;
	};
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_PrototypeName()
{
	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
		return E_FAIL;
	Safe_AddRef(pFileLoader);

	// 리틀본 //
	//const _tchar* pPrototypeName = TEXT("Prototype_GameObject_Skill_ThrowSkul");
	//pFileLoader->Add_PrototypeName(L"두개골투척", pPrototypeName);

	//if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillThrowSkul::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	//pPrototypeName = TEXT("Prototype_GameObject_Skill_Headless");
	//pFileLoader->Add_PrototypeName(L"머리가본체", pPrototypeName);

	//if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillHeadless::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	// 그림 리퍼 //
	const _tchar* pPrototypeName = TEXT("Prototype_GameObject_Skill_GateOfNether");
	pFileLoader->Add_PrototypeName(L"명계의균열", pPrototypeName);
	
	if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillGateOfNether::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pFileLoader);
		return E_FAIL;
	}

	pPrototypeName = TEXT("Prototype_GameObject_Skill_Guillotine");
	pFileLoader->Add_PrototypeName(L"길로틴", pPrototypeName);

	if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillGuillotine::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pFileLoader);
		return E_FAIL;
	}

	// 워터 스컬 //
	//pPrototypeName = TEXT("Prototype_GameObject_Skill_Dive");
	//pFileLoader->Add_PrototypeName(L"입수", pPrototypeName);

	//if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillDive::Create(m_pDevice, m_pContext))))
	//{
	//	Safe_Release(pFileLoader);
	//	return E_FAIL;
	//}

	//pPrototypeName = TEXT("Prototype_GameObject_Skill_HighTide");
	//pFileLoader->Add_PrototypeName(L"밑물", pPrototypeName);

	//if (FAILED(m_pGameInstance->Add_Prototype(pPrototypeName, CSkillHighTide::Create(m_pDevice, m_pContext))))
	//{
	//	Safe_Release(pFileLoader);
	//	return E_FAIL;
	//}

	Safe_Release(pFileLoader);

	return S_OK;
}

HRESULT CMainApp::Add_SpriteObject(const _tchar* pFileName, const SPRITE_INFO& tSprite)
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

	/** @note - 싱글톤도 new로 동적할당했기에 꼭 직접 해제를 명시해줘야함 (delete 해줘야 한다는 뜻) */
	CPlayer_Manager::GetInstance()->DestroyInstance();
	CStage_Manager::GetInstance()->DestroyInstance();
	CInputHandler::GetInstance()->DestroyInstance();
	CFileLoader::GetInstance()->DestroyInstance();
	CUI_Handler::GetInstance()->DestroyInstance();
	CGameInstance::Release_Engine();

	//해제안된 COM객체 추적
#ifdef _DEBUG
	list_remaining_d3d_objects();
#endif
}

