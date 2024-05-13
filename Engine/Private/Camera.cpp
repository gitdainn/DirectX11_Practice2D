#include "..\Public\Camera.h"
#include "PipeLine.h"


CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);

}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);

}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof m_CameraDesc);

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Set_TransformDesc(m_CameraDesc.tTransformDesc);

	/*_vector		vPosition = XMLoadFloat3(&m_CameraDesc.vEye);
	vPosition = XMVectorSetW(vPosition, 1.f);*/

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));	

	return S_OK;
}

_uint CCamera::Tick(_double TimeDelta)
{
	if (nullptr != m_pPipeLine)
	{
		/* 카메라 월드변환행렬의 역행렬 == 뷰스페이스 변환행렬. */
		m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW,
			m_pTransformCom->Get_WorldMatrix_Inverse());

		m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ,
			XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
	}


	return __super::Tick(TimeDelta);
}

_uint CCamera::LateTick(_double TimeDelta)
{



	return __super::LateTick(TimeDelta);
}

HRESULT CCamera::Render()
{
	return S_OK;
}

CGameObject* CCamera::Clone(const tSpriteInfo& SpriteInfo, void* pArg) const
{
	return nullptr;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransformCom);

}
