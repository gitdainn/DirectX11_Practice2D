#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "ColliderAABB2D.h"

CColliderAABB2D::CColliderAABB2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
{
}

CColliderAABB2D::CColliderAABB2D(const CColliderAABB2D& rhs)
	: CCollider(rhs)
{
}

HRESULT CColliderAABB2D::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderAABB2D::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	_matrix ScaleMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.y, m_tColliderDesc.vScale.z);
	TranslationMatrix = XMMatrixTranslation(m_tColliderDesc.vPosition.x, m_tColliderDesc.vPosition.y, m_tColliderDesc.vPosition.z);
	
	//m_pAABB_Original->Transform(*m_pAABB_Original, ScaleMatrix * TranslationMatrix);
	//m_pAABB = new BoundingBox(*m_pAABB_Original);

	return S_OK;
}

void CColliderAABB2D::Tick(_fmatrix TransformMatrix)
{
	__super::Tick(TransformMatrix);
	//m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));

	m_bIsCollision = false;
}

_bool CColliderAABB2D::IsCollision(CColliderAABB2D* pTarget) const
{
	COLLIDER_DESC tTargetDesc = pTarget->Get_ColliderDesc();

	_float2 vRadius = _float2(m_tColliderDesc.vScale.x / 2, m_tColliderDesc.vScale.y / 2);
	_float2 vTargetRadius = _float2(tTargetDesc.vScale.x / 2, tTargetDesc.vScale.y / 2);

	const _float fSumRadiusX = vRadius.x + vTargetRadius.x;
	const _float fDistanceX = abs(m_tColliderDesc.vPosition.x - tTargetDesc.vPosition.x);

	if (fSumRadiusX > fDistanceX)
		return true;

	return false;
}

_bool CColliderAABB2D::IsCollision(CColliderOBB2D* pTarget) const
{
	return _bool();
}

_bool CColliderAABB2D::IsCollision(CColliderSphere2D* pTarget) const
{
	return _bool();
}

CColliderAABB2D* CColliderAABB2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderAABB2D* pInstance = new CColliderAABB2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CColliderAABB2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CColliderAABB2D::Clone(void* pArg)
{
	CColliderAABB2D* pInstance = new CColliderAABB2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CColliderAABB2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderAABB2D::Free()
{
	__super::Free();
}