#include "..\Public\Bone.h"
#include "Model.h"

CBone::CBone()
{

}

CBone::CBone(const CBone & rhs)
	: m_iParentIndex(rhs.m_iParentIndex)
	, m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_OffSetMatrix(rhs.m_OffSetMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)	
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CBone::Initialize(aiNode * pAINode, CModel* pModel, CBone* pParent)
{
	strcpy_s(m_szName, pAINode->mName.data);

	if(nullptr != pParent)
		m_iParentIndex = pModel->Get_BoneIndex(pParent->m_szName);

	/* 이 뼈가 가지고 있어야하는 기초 상태의 mTransformation을 저장한다. */	
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffSetMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::SetUp_TransformationMatrix(_fmatrix Matrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, Matrix);
}

void CBone::SetUp_CombinedTransformationMatrix()
{
	if (nullptr == m_pParent)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
		return;
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, 
		XMLoadFloat4x4(&m_TransformationMatrix) *
		XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
}

void CBone::SetUp_OffsetMatrix(_fmatrix Matrix)
{
	XMStoreFloat4x4(&m_OffSetMatrix, Matrix);
}

CBone * CBone::Create(aiNode * pAINode, CModel* pModel, CBone* pParent)
{
	CBone *	pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pModel, pParent)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}
