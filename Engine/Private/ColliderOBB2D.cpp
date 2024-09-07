#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "ColliderAABB2D.h"
#include "ColliderOBB2D.h"
#include "ColliderSphere2D.h"

CColliderOBB2D::CColliderOBB2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
{
}

CColliderOBB2D::CColliderOBB2D(const CColliderOBB2D& rhs)
	: CCollider(rhs)
{
}

HRESULT CColliderOBB2D::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderOBB2D::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	_matrix ScaleMatrix, RotationXMatrix, RotationYMatrix, RotationZMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.y, m_tColliderDesc.vScale.z);
	RotationXMatrix = XMMatrixRotationX(m_tColliderDesc.vRotation.x);
	RotationYMatrix = XMMatrixRotationY(m_tColliderDesc.vRotation.y);
	RotationZMatrix = XMMatrixRotationZ(m_tColliderDesc.vRotation.z);
	TranslationMatrix = XMMatrixTranslation(m_tColliderDesc.vPosition.x, m_tColliderDesc.vPosition.y, m_tColliderDesc.vPosition.z);

	//m_pOBB_Original->Transform(*m_pOBB_Original, RotationXMatrix * RotationYMatrix * RotationZMatrix * TranslationMatrix);
	//m_pOBB_Original->Extents = _float3(m_tColliderDesc.vScale.x * 0.5f, m_tColliderDesc.vScale.y * 0.5f, m_tColliderDesc.vScale.z * 0.5f);
	//m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);

	return S_OK;
}

void CColliderOBB2D::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);

	m_bIsCollision = false;
}

_bool CColliderOBB2D::Intersects(CColliderAABB2D* pTarget) const
{
	return _bool();
}

_bool CColliderOBB2D::Intersects(CColliderOBB2D* pTarget) const
{
	return _bool();
}

_bool CColliderOBB2D::Intersects(CColliderSphere2D* pTarget) const
{
	return _bool();
}

CColliderOBB2D* CColliderOBB2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderOBB2D* pInstance = new CColliderOBB2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CColliderOBB2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CColliderOBB2D::Clone(void* pArg)
{
	CColliderOBB2D* pInstance = new CColliderOBB2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CColliderOBB2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderOBB2D::Free()
{
	__super::Free();
}