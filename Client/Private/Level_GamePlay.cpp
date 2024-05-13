#include "stdafx.h"

#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임 플레이 레벨임"));
#endif
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(5.f, 3.f, 5.f, 1.f);
	LightDesc.fRange = 7.f;
	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;


	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 5.f, 1.f);
	LightDesc.fRange = 7.f;
	LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.0f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERA_DYNAMIC_DESC		CameraDynamicDesc;
	ZeroMemory(&CameraDynamicDesc, sizeof CameraDynamicDesc);

	CameraDynamicDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDynamicDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDynamicDesc.CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDynamicDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDynamicDesc.CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
	CameraDynamicDesc.CameraDesc.fNear = 0.2f;
	CameraDynamicDesc.CameraDesc.fFar = 300;

	CameraDynamicDesc.CameraDesc.tTransformDesc.SpeedPerSec = 20.f;
	CameraDynamicDesc.CameraDesc.tTransformDesc.RotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDynamicDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ForkLift"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;	

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < 20; ++i)
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Monster"), LEVEL_GAMEPLAY, pLayerTag)))
			return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Effect_Blue"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Effect_Red"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	for (_uint i = 0; i < 30; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Explosion"), LEVEL_GAMEPLAY, pLayerTag)))
			return E_FAIL;
	}






	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
