#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "ColliderSphere2D.h"

CColliderSphere2D::CColliderSphere2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCollider(pDevice, pContext)
{
}

CColliderSphere2D::CColliderSphere2D(const CColliderSphere2D& rhs)
	: CCollider(rhs)
{
}

HRESULT CColliderSphere2D::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderSphere2D::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	_matrix ScaleMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.y, m_tColliderDesc.vScale.z);
	TranslationMatrix = XMMatrixTranslation(m_tColliderDesc.vPosition.x, m_tColliderDesc.vPosition.y, m_tColliderDesc.vPosition.z);

	//m_pSphere_Original->Transform(*m_pSphere_Original, ScaleMatrix * TranslationMatrix);
	//m_pSphere = new BoundingSphere(*m_pSphere_Original);

	return S_OK;
}

void CColliderSphere2D::Tick(_fmatrix TransformMatrix)
{
	__super::Tick(TransformMatrix);

	//m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);

	m_bIsCollision = false;
}

_bool CColliderSphere2D::IsCollision(CColliderAABB2D* pTarget) const
{
	return _bool();
}

_bool CColliderSphere2D::IsCollision(CColliderOBB2D* pTarget) const
{
	return _bool();
}

_bool CColliderSphere2D::IsCollision(CColliderSphere2D* pTarget) const
{
	return _bool();
}

CColliderSphere2D* CColliderSphere2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderSphere2D* pInstance = new CColliderSphere2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CColliderSphere2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CColliderSphere2D::Clone(void* pArg)
{
	CColliderSphere2D* pInstance = new CColliderSphere2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CColliderSphere2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderSphere2D::Free()
{
	__super::Free();
}