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

	CCamera_Dynamic::CAMERA_DYNAMIC_DESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof tCameraDesc);

	tCameraDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	tCameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tCameraDesc.CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	tCameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	tCameraDesc.CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	tCameraDesc.CameraDesc.fNear = 0.2f;
	tCameraDesc.CameraDesc.fFar = 300;

	tCameraDesc.CameraDesc.tTransformDesc.SpeedPerSec = 20.f;
	tCameraDesc.CameraDesc.tTransformDesc.RotationPerSec = XMConvertToRadians(180.0f);

	tCameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);

	if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TOOL, LAYER_CAMERA, &tCameraDesc))))
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
