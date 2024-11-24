#include "..\Public\Level.h"
#include "Object_Manager.h"

CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

HRESULT CLevel::Late_Initialize()
{
	CObject_Manager* pObject_Manager = CObject_Manager::GetInstance();
	if (nullptr == pObject_Manager)
		return E_FAIL;
	Safe_AddRef(pObject_Manager);
	pObject_Manager->Late_Initialize();
	Safe_Release(pObject_Manager);

	return S_OK;
}

void CLevel::Tick(_double TimeDelta)
{
	
}

void CLevel::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
