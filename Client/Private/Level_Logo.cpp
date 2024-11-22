#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "FileLoader.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Camera_Dynamic.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_Priority()))
		return E_FAIL;

	///* 검색시에 어떤 레벨에 있는 특정 태그에 있는 몇번째 녀석. */
	if (FAILED(Ready_Layer_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
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

#pragma region 라인 데이터
	if (FAILED(pFileLoader->Load_Line(TEXT("../Bin/DataFiles/Line.data"), m_pDevice, m_pContext)))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Line FAILED");
		Safe_Release(pFileLoader);
		return E_FAIL;
	}
#pragma endregion

#pragma region 오브젝트 데이터
	if (FAILED(pFileLoader->Load_FIle(TEXT("../Bin/DataFiles/Map.data"), LEVEL_LOGO)))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_FIle FAILED");
	 Safe_Release(pFileLoader);
		return E_FAIL;
	}
#pragma endregion

#pragma region UI 데이터
	if (FAILED(pFileLoader->Load_ObjectTransform_Excel(TEXT("../Bin/DataFiles/UI.xlsx"))))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		Safe_Release(pFileLoader);
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_ComponentInfo_Excel(TEXT("../Bin/DataFiles/UI.xlsx"))))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		Safe_Release(pFileLoader);
		return E_FAIL;
	}

	if (FAILED(pFileLoader->Load_Excel(TEXT("../Bin/DataFiles/UI.xlsx"), LEVEL::LEVEL_LOGO)))
	{
		MSG_BOX("CLevel_Logo - Initialize() - Load_Excel FAILED");
		Safe_Release(pFileLoader);
		return E_FAIL;
	}
#pragma endregion

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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GrimReaper"), LEVEL_LOGO, LAYER_PLAYER, tSpriteInfo)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LittleBorn"), LEVEL_LOGO, LAYER_PLAYER, tSpriteInfo)))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Solider"), LEVEL_LOGO, LAYER_ENEMY, tSpriteInfo)))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}

	//tSpriteInfo.fSize = _float2{ 30, 30.f };
	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkulItem"), LEVEL_LOGO, LAYER_ITEM, tSpriteInfo)))
	//{
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;		
	//}

	Safe_Release(pGameInstance);
	return S_OK;

}

HRESULT CLevel_Logo::Ready_Layer_Camera()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERA_DYNAMIC_DESC		tCameraDynamicDesc;
	ZeroMemory(&tCameraDynamicDesc, sizeof CCamera_Dynamic::CAMERA_DYNAMIC_DESC);

	tCameraDynamicDesc.tCameraDesc.vEye = _float4(0.f, 0.f, -500.f, 1.f);
	tCameraDynamicDesc.tCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tCameraDynamicDesc.tCameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	tCameraDynamicDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraDynamicDesc.tCameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	tCameraDynamicDesc.tCameraDesc.fNear = 1.f;
	tCameraDynamicDesc.tCameraDesc.fFar = 1000.f;

	tCameraDynamicDesc.tCameraDesc.tTransformDesc.SpeedPerSec = 50.f;
	tCameraDynamicDesc.tCameraDesc.tTransformDesc.RotationPerSec = XMConvertToRadians(180.0f);

	const _vector	vMapScaleLTRB[4] = { {-688.f, 0.f, 0.f, 0.f}
										,{0.f, 398.f, 0.f, 0.f}
										,{982.f, 0.f, 0.f, 0.f}
										,{0.f, -632.f, 0.f, 0.f} }; // 맵 크기 Left Top Right Bottom 순
	_float fLimitWidth = XMVectorGetX(XMVector4Length(vMapScaleLTRB[2] - vMapScaleLTRB[0]));
	_float fLimitHeight = XMVectorGetY(XMVector4Length(vMapScaleLTRB[1] - vMapScaleLTRB[3]));
	tCameraDynamicDesc.tCameraLimitRange.vRadius = _float2(fLimitWidth / 2.f, fLimitHeight / 2.f);
	tCameraDynamicDesc.tCameraLimitRange.vCenterPos = _float2(XMVectorGetX(vMapScaleLTRB[2]) - fLimitWidth / 2.f
															, XMVectorGetY(vMapScaleLTRB[1]) - fLimitHeight / 2.f);


	if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_STATIC, TEXT("Layer_Camera"), &tCameraDynamicDesc))))
	{
		Safe_Release(pGameInstance);
		MSG_BOX("CLevel_Logo - Ready_Layer_Camera() - NULL");
		return E_FAIL;
	}

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
