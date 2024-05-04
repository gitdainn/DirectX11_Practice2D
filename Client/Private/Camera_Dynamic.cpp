#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"

#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
	ZeroMemory(&m_CameraDynamicDesc, sizeof(m_CameraDynamicDesc));
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
	, m_CameraDynamicDesc(rhs.m_CameraDynamicDesc)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void * pArg)
{
	memcpy(&m_CameraDynamicDesc, pArg, sizeof m_CameraDynamicDesc);

	if (FAILED(__super::Initialize(&m_CameraDynamicDesc.CameraDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Dynamic::Tick(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//if (pGameInstance->Get_KeyState(DIK_W))
	//	m_pTransform->Go_Straight(TimeDelta);

	//if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	//	m_pTransform->Go_Backward(TimeDelta);

	//if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	//	m_pTransform->Go_Left(TimeDelta);

	//if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	//	m_pTransform->Go_Right(TimeDelta);

	//_long MouseMove = { 0 };

	//if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
	//{
	//	m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * (MouseMove * 0.1f));
	//}

	//if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
	//{
	//	m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta * (MouseMove * 0.1f));
	//}


	Safe_Release(pGameInstance);


	__super::Tick(TimeDelta);

	return _uint();
}

_uint CCamera_Dynamic::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	return _uint();
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg) const
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();


}
