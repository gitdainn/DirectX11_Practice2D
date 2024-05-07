#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#pragma region ENGINE_HEADERS
#include "GameInstance.h"
#pragma endregion

#pragma region TPP:_HEADERS
#include "Loader.h"
#include "Level_Tool.h"
#pragma endregion

USING(Tool)

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 로더안에서 스레드를 생성한다. */
	/* 생성한 스레드가 필요한 준비를 한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (true == m_pLoader->Get_Finished())
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CLevel* pNewLevel = { nullptr };

			switch (m_eNextLevelID)
			{
			case LEVEL_TOOL:
				pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
				break;

			default:
				break;
			}

			if (nullptr == pNewLevel)
			{
				Safe_Release(pGameInstance);
				return;
			}

			if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pNewLevel)))
			{
				Safe_Release(pGameInstance);
				return;
			}

			Safe_Release(pGameInstance);
		}
	}

#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
#endif


}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}
