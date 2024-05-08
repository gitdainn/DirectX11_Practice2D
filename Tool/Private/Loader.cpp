#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "InstallObject.h"
#include "Utility.h"

USING(Tool)

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = (CLoader*)pArg;

	/** @note - C2102 &에 l-value가 와야합니다. 라는 오류 발생 시 
	* -> EnterCriticalSection(&pLoader->Get_CriticalSection()); 시 발생함.
	* 주소 참조(&)는 임시 값에 불가능하다는 뜻이므로 변수에 값을 담고, 해당 변수의 주소를 참조하면 됨 */
	CRITICAL_SECTION CriticalSection = pLoader->Get_CriticalSection();
	const LPCRITICAL_SECTION pCritical = &CriticalSection;
	EnterCriticalSection(pCritical);

	HRESULT			hr = { 0 };

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_TOOL: /* 로딩씬 다음레벨이 로고다. 로고레벨에 필요한 사전 생성(리소스, 원형객체) 작업을 하자. */
		hr = pLoader->Loading_ForToolLevel();
		break;

	default:
		break;
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(pCritical);
		return 1;
	}

	LeaveCriticalSection(pCritical);

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

HRESULT CLoader::Loading_ForToolLevel()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LEVEL eLevel = LEVEL_TOOL;
#pragma region TEXTURES	
	wsprintf(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For.Prototype_Component_Sprite_GrimReaper */
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Sprite_Tile"),
		CUtility::Load_Texture_Folder(m_pDevice, m_pContext, TEXT("../Bin/Resources/Tiles/ForestTile/")))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

#pragma region OBJECT
	/* For.Prototype_GameObject_Install */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Install"),
		CInstallObject::Create(m_pDevice, m_pContext))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	};
#pragma endregion

	wsprintf(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

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
