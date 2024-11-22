#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "Player_Manager.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
	ZeroMemory(&m_tCameraDynamicDesc, sizeof(m_tCameraDynamicDesc));
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
	, m_tCameraDynamicDesc(rhs.m_tCameraDynamicDesc)
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
	memcpy(&m_tCameraDynamicDesc, pArg, sizeof m_tCameraDynamicDesc);

	if (FAILED(__super::Initialize(&m_tCameraDynamicDesc.tCameraDesc)))
		return E_FAIL;

	return S_OK;
}

// 스컬이 교체되니까.. 이걸 딴 곳에서 셋팅해줘야할듯함.
HRESULT CCamera_Dynamic::Late_Initialize(void* pArg)
{
	CPlayer_Manager* pPlayerManager = CPlayer_Manager::GetInstance();
	Safe_AddRef(pPlayerManager);

	m_pOwner = pPlayerManager->Get_MainSkul();
	if (nullptr == m_pOwner)
	{
		Safe_Release(pPlayerManager);
		return E_FAIL;
	}
	Safe_Release(pPlayerManager);

	return S_OK;
}

_uint CCamera_Dynamic::Tick(_double TimeDelta)
{
	Follow_Owner();

	LimitCameraToArea();

	return __super::Tick(TimeDelta);
}

_uint CCamera_Dynamic::LateTick(_double TimeDelta)
{
	

	return __super::LateTick(TimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

void CCamera_Dynamic::Follow_Owner()
{
	if (nullptr == m_pOwner)
		return;

	CTransform* pTransformCom = m_pOwner->Get_TransformCom();
	if (nullptr == pTransformCom)
		return;

	_vector vOwnerPosition = pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCameraPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 플레이어의 위치로 셋팅하되, 카메라 위치는 보존합니다. (카메라 위치와 플레이어 위치가 동일하면 아무것도 안 보임)
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetZ(vOwnerPosition, XMVectorGetZ(vCameraPosition)));
	m_pTransformCom->LookAt(vOwnerPosition);
}

void CCamera_Dynamic::LimitCameraToArea()
{
	_float2 vMapCenter = m_tCameraDynamicDesc.tCameraLimitRange.vCenterPos;
	_float2 vMapRadius = m_tCameraDynamicDesc.tCameraLimitRange.vRadius;

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float2 vRadius = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);

	_float fLX = vMapRadius.x - vRadius.x;
	_float fClampX = CLAMP(XMVectorGetX(vPosition), -fLX + vMapCenter.x, fLX + vMapCenter.x);

	_float fLY = vMapRadius.y - vRadius.y;
	_float fClampY = CLAMP(XMVectorGetY(vPosition), -fLY + vMapCenter.y, fLY + vMapCenter.y);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{fClampX, fClampY, XMVectorGetZ(vPosition), 1.f });
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
