#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "FileLoader.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_Priority()))
		return E_FAIL;

	/* 검색시에 어떤 레벨에 있는 특정 태그에 있는 몇번째 녀석. */
	if (FAILED(Ready_Layer_GameObject()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	/* 로고레벨다음은 게임플레이야. */
	// if (GetKeyState(VK_SPACE) & 0x8000)
	// {
	// 	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	// 	Safe_AddRef(pGameInstance);
	// 
	// 	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
	// 		return;
	// 
	// 	Safe_Release(pGameInstance);
	// }

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Logo Level"));
#endif
}

HRESULT CLevel_Logo::Ready_Layer_Priority()
{
	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
	{
		MSG_BOX("CLevel_Logo - Intialize() - FileLoader is NULL");
		return E_FAIL;
	}
	Safe_AddRef(pFileLoader);

	if (FAILED(pFileLoader->Load_Line(TEXT("../Bin/DataFiles/Line.data"), m_pDevice, m_pContext)))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Line FAILED");
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_ObjectTransform_Excel(TEXT("../Bin/DataFiles/Level_Logo.xlsx"))))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_ComponentInfo_Excel(TEXT("../Bin/DataFiles/Level_Logo.xlsx"))))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_Excel(TEXT("../Bin/DataFiles/Level_Logo.xlsx"), LEVEL::LEVEL_LOGO)))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		return E_FAIL;
	}

	Safe_Release(pFileLoader);
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_GameObject()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	SPRITE_INFO tSpriteInfo;
	tSpriteInfo.fSize = _float2{ 300.f, 300.f };

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_LOGO, LAYER_BACKGROUND, tSpriteInfo)))
	//	return E_FAIL;

	//tSpriteInfo.fSize = _float2{ 300.f, 320.f };
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GrimReaper"), LEVEL_LOGO, LAYER::PLAYER, tSpriteInfo)))
		return E_FAIL;
	CPlayer_Manager::GetInstance()->Set_SubSkul(dynamic_cast<CPlayer*>(pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER::PLAYER)->back()));

	//tSpriteInfo.fSize = _float2{ 200.f, 150.f };
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterSkul"), LEVEL_LOGO, LAYER::PLAYER, tSpriteInfo)))
		return E_FAIL;
	CPlayer_Manager::GetInstance()->Set_MainSkul(dynamic_cast<CPlayer*>(pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER::PLAYER)->back()));

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LittleBorn"), LEVEL_LOGO, LAYER::PLAYER, tSpriteInfo)))
	//	return E_FAIL;
	//CPlayer_Manager::GetInstance()->Set_MainSkul(dynamic_cast<CPlayer*>(pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER::PLAYER)->back()));

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Solider"), LEVEL_LOGO, LAYER::ENEMY, tSpriteInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;

}


CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
