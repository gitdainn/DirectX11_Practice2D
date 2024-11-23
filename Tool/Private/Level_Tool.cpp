#include "stdafx.h"
#include "Level_Tool.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()

{
	/* 검색시에 어떤 레벨에 있는 특정 태그에 있는 몇번째 녀석. */
	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObject()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tool::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Logo Level"));
#endif
}

HRESULT CLevel_Tool::Ready_Layer_Camera()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERA_DYNAMIC_DESC		tCameraDynamicDesc;
	ZeroMemory(&tCameraDynamicDesc, sizeof tCameraDynamicDesc);


	tCameraDynamicDesc.tCameraDesc.vEye = _float4(0.f, 0.f, -500.f, 1.f);
	tCameraDynamicDesc.tCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tCameraDynamicDesc.tCameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	tCameraDynamicDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraDynamicDesc.tCameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	tCameraDynamicDesc.tCameraDesc.fNear = 1.f;
	tCameraDynamicDesc.tCameraDesc.fFar = 1000.f;

	tCameraDynamicDesc.tCameraDesc.tTransformDesc.SpeedPerSec = 50.f;
	tCameraDynamicDesc.tCameraDesc.tTransformDesc.RotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TOOL, TEXT("Layer_Camera"), &tCameraDynamicDesc))))
	{
		MSG_BOX("CLevel_Tool - Ready_Layer_Camera() - NULL");
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_GameObject()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
}
