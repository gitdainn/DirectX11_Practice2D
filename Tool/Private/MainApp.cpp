#include "stdafx.h"

#include "MyImGui.h"

#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InstallObject.h"
#include "CollisionManager.h"
#include "Utility.h"

USING(Tool)

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pImGui(CMyImGui::GetInstance())
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

	/** @note - m_pDevice와 m_pContext가 할당되는 곳 */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Sprite_For_Static()))
		return E_FAIL;

	if (FAILED(SetUp_StartLevel(LEVEL_TOOL)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bazzi"), TEXT("../Bin/Resources/Fonts/133ex.SpriteFont"))))
		return E_FAIL;

	m_pImGui->Initialize(m_pDevice, m_pContext);

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	m_pImGui->Tick(TimeDelta);
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
	m_pGameInstance->Render_Collider();

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

	m_pImGui->Render();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(LEVEL eNextLevelID)
{
	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevelID));

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

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject_For_Static()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Prototype_GameObject_Install */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Install"),
		CInstallObject::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Sprite_For_Static()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	///* For.Prototype_Component_Sprite_LittleBorn */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_LittleBorn"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/LittleBorn/Wait_%d.png"), 48))))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//};

	///* For.Prototype_Component_Sprite_GrimReaper */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GrimReaperUV"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/GrimReaper/GrimReaperUV_%d.png")))))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//};

	///* For.Prototype_Component_Sprite_WaterSkul */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_WaterSkulUV"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/WaterSkul/WaterSkulUV_%d.png")))))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//};

	/* For.Prototype_Component_Sprite_Tile */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_ForestTile"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Tiles/ForestTile/")))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_ForestEnvironment */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_ForestEnvironment"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Environment/Forest/")))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_Component_Sprite_Background */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Background"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Background/")))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	Safe_Release(pGameInstance);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

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

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pImGui);

	CGameInstance::Release_Engine();
}

