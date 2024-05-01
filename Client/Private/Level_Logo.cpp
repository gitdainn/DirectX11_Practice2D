#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "PlayerInfo.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
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

HRESULT CLevel_Logo::Ready_Layer_GameObject()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	SPRITE_INFO tSpriteInfo;

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround"), LEVEL_LOGO, LAYER_BACKGROUND, tSpriteInfo)))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_GrimReaper"), LEVEL_LOGO, LAYER_PLAYER, tSpriteInfo)))
		return E_FAIL;
	CPlayerInfo::GetInstance()->Set_EquippedSkul(dynamic_cast<CPlayer*>(pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER_PLAYER)->back()));

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LittleBorn"), LEVEL_LOGO, LAYER_PLAYER, tSpriteInfo)))
		return E_FAIL;
	CPlayerInfo::GetInstance()->Set_HoldingSkul(dynamic_cast<CPlayer*>(pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER_PLAYER)->back()));

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
