#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Camera_Dynamic.h"
#include "Utility.h"

#pragma region PLAYER
#include "GrimReaper.h"
#include "LittleBorn.h"
#include "WaterSkul.h"
#pragma endregion

#pragma region ENEMY
#include "Solider.h"
#pragma endregion

#pragma region ETC
#include "BackGround.h"
#include "Tile.h"
#include "Environment.h"
#include "DisplayUI.h"
#include "SkulItem.h"
#pragma endregion

#pragma region COMPONENT
#include "ColliderAABB2D.h"
#pragma endregion

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{	
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	HRESULT			hr = { 0 };

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO: /* 로딩씬 다음레벨이 로고다. 로고레벨에 필요한 사전 생성(리소스, 원형객체) 작업을 하자. */
		hr = pLoader->Loading_ForLogoLevel();
		break;

	case LEVEL_GAMEPLAY:
		hr = pLoader->Loading_ForGamePlayLevel();
		break;
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(&pLoader->Get_CriticalSection());
		return 1;
	}
	
	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	/* 스레드를 생성한다. */
	/* 스레드를 생성하게되면 진입점함수를 정의해야해. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;	

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	if (FAILED(Loading_Component_Logo()))
		return E_FAIL;
	
	if (FAILED(Loaiding_GameObject_Logo()))
		return E_FAIL;

	wsprintf(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Component_Logo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loaiding_GameObject_Logo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	wsprintf(m_szLoadingText, TEXT("객체원형을 로딩중."));

#pragma region PLAYER
	/* For.Prototype_GameObject_GrimReaper */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrimReaper"),
		CGrimReaper::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_GameObject_LittleBorn */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LittleBorn"),
		CLittleBorn::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_GameObject_WaterSkul */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterSkul"),
		CWaterSkul::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

#pragma region ENEMY
		/* For.Prototype_GameObject_Solider */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Solider"),
			CSolider::Create(m_pDevice, m_pContext))))
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		};
#pragma endregion

#pragma region ENVIRONMENT
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_GameObject_Tile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tile"),
		CTile::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};

	/* For.Prototype_GameObject_Environment */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Environment"),
		CEnvironment::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

#pragma region UI
	/* For.Prototype_GameObject_DisplayUI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DisplayUI"),
		CDisplayUI::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

#pragma region ITEM
	/* For.Prototype_GameObject_SkulItem */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkulItem"),
		CSkulItem::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*  */
//#pragma region TEXTURES
//#pragma endregion
//
//
//#pragma region VIBUFFER
//	/* For.Prototype_Component_VIBuffer_Cube */
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
//		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
//		return E_FAIL;
//	
//#pragma endregion
//
//	wsprintf(m_szLoadingText, TEXT("콜라이더를 로딩중입니다."));
//#pragma region COLLIDER
//	/* For.Prototype_Component_Collider_AABB*/
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
//		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Collider_OBB */
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
//		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
//		return E_FAIL;
//
//	/* For.Prototype_Component_Collider_SPHERE */
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
//		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
//		return E_FAIL;
//
//#pragma endregion
//
//
//#pragma region SHADERS
//	wsprintf(m_szLoadingText, TEXT("셰이더를 로딩중입니다."));
//	/* For.Prototype_Component_Shader_VtxNorTex */
//	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
//		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
//		return E_FAIL;
//#pragma endregion
//
//
//#pragma region GAMEOBJECTS
//	wsprintf(m_szLoadingText, TEXT("객체원형을 로딩중."));
//
//#pragma endregion

	Safe_Release(pGameInstance);

	wsprintf(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;	
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);	

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
