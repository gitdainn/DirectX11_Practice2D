#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_pSphere_Original(nullptr == rhs.m_pSphere_Original ? rhs.m_pSphere_Original : new BoundingSphere(*rhs.m_pSphere_Original))
	, m_pAABB_Original(nullptr == rhs.m_pAABB_Original ? rhs.m_pAABB_Original : new BoundingBox(*rhs.m_pAABB_Original))
	, m_pOBB_Original(nullptr == rhs.m_pOBB_Original ? rhs.m_pOBB_Original : new BoundingOrientedBox(*rhs.m_pOBB_Original))
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif _DEBUG
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.0f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif // _DEBUG
	
	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	COLLIDERDESC	ColliderDesc = *(COLLIDERDESC*)pArg;

	_matrix ScaleMatrix, RotationXMatrix, RotationYMatrix, RotationZMatrix, TranslationMatrix;

	ScaleMatrix = XMMatrixScaling(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);
	RotationXMatrix = XMMatrixRotationX(ColliderDesc.vRotation.x);
	RotationYMatrix = XMMatrixRotationY(ColliderDesc.vRotation.y);
	RotationZMatrix = XMMatrixRotationZ(ColliderDesc.vRotation.z);
	TranslationMatrix = XMMatrixTranslation(ColliderDesc.vPosition.x, ColliderDesc.vPosition.y, ColliderDesc.vPosition.z);

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB_Original, ScaleMatrix * TranslationMatrix);
		m_pAABB = new BoundingBox(*m_pAABB_Original);
		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB_Original, RotationXMatrix * RotationYMatrix * RotationZMatrix * TranslationMatrix);
		m_pOBB_Original->Extents = _float3(ColliderDesc.vScale.x * 0.5f, ColliderDesc.vScale.y* 0.5f, ColliderDesc.vScale.z * 0.5f);
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
		break;

	case CCollider::TYPE_SPHERE:		
		m_pSphere_Original->Transform(*m_pSphere_Original, ScaleMatrix * TranslationMatrix);
		m_pSphere = new BoundingSphere(*m_pSphere_Original);
		break;
	}	
		
	return S_OK;
}

void CCollider::Tick(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
		break;
	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
		break;
	case CCollider::TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
		break;
	}

	m_isCollision = false;
}

_bool CCollider::Collision(CCollider * pTarget)
{
	if (TYPE_AABB == m_eType)
	{
		if (TYPE_AABB == pTarget->m_eType)
			m_isCollision = m_pAABB->Intersects(*pTarget->m_pAABB);
		else if (TYPE_OBB == pTarget->m_eType)
			m_isCollision = m_pAABB->Intersects(*pTarget->m_pOBB);
		else if (TYPE_SPHERE == pTarget->m_eType)
			m_isCollision = m_pAABB->Intersects(*pTarget->m_pSphere);
	}

	else if (TYPE_OBB == m_eType)
	{
		if (TYPE_AABB == pTarget->m_eType)
			m_isCollision = m_pOBB->Intersects(*pTarget->m_pAABB);
		else if (TYPE_OBB == pTarget->m_eType)
			m_isCollision = m_pOBB->Intersects(*pTarget->m_pOBB);
		else if (TYPE_SPHERE == pTarget->m_eType)
			m_isCollision = m_pOBB->Intersects(*pTarget->m_pSphere);
	}

	else if (TYPE_SPHERE == m_eType)
	{
		if (TYPE_AABB == pTarget->m_eType)
			m_isCollision = m_pSphere->Intersects(*pTarget->m_pAABB);
		else if (TYPE_OBB == pTarget->m_eType)
			m_isCollision = m_pSphere->Intersects(*pTarget->m_pOBB);
		else if (TYPE_SPHERE == pTarget->m_eType)
			m_isCollision = m_pSphere->Intersects(*pTarget->m_pSphere);
	}

	if (true == m_isCollision)
		pTarget->m_isCollision = true;

	return m_isCollision;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	
	CPipeLine*		pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	_vector		vColor = true == m_isCollision ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);	

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, vColor);
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, vColor);
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, vColor);
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[2]));

	return Matrix;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider *	pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider *	pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pOBB_Original);

	Safe_Delete(m_pSphere);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
}
