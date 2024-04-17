#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Player.h"

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
	case LEVEL_LOGO: /* �ε��� ���������� �ΰ��. �ΰ����� �ʿ��� ���� ����(���ҽ�, ������ü) �۾��� ����. */
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

	/* �����带 �����Ѵ�. */
	/* �����带 �����ϰԵǸ� �������Լ��� �����ؾ���. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;	

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	/*  */
#pragma region TEXTURES	
	wsprintf(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Background/Forest_%d.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Sprite_LittleBorn */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Sprite_LittleBorn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/LittleBorn/Wait_%d.png"), 48))))
		return E_FAIL;
#pragma endregion

#pragma region MODELS
	wsprintf(m_szLoadingText, TEXT("�𵨸� �ε����Դϴ�."));
	

#pragma endregion

#pragma region SHADERS
	wsprintf(m_szLoadingText, TEXT("���̴��� �ε����Դϴ�."));
	for (_uint i = 0; i < 999999999; ++i)
	{
		int a = 10;
	}
#pragma endregion

#pragma region GAMEOBJECTS
	wsprintf(m_szLoadingText, TEXT("��ü������ �ε���."));

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

#pragma endregion

	wsprintf(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));
	m_isFinished = true;

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*  */
#pragma region TEXTURES
#pragma endregion


#pragma region VIBUFFER
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
#pragma endregion

	wsprintf(m_szLoadingText, TEXT("�ݶ��̴��� �ε����Դϴ�."));
#pragma region COLLIDER
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

#pragma endregion


#pragma region SHADERS
	wsprintf(m_szLoadingText, TEXT("���̴��� �ε����Դϴ�."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
#pragma endregion


#pragma region GAMEOBJECTS
	wsprintf(m_szLoadingText, TEXT("��ü������ �ε���."));

#pragma endregion

	Safe_Release(pGameInstance);

	wsprintf(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));
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
