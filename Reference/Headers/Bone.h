#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;
public:
	_uint Get_ParentIndex() const {
		return m_iParentIndex;
	}

	const char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_OffsetMatrix() {
		return XMLoadFloat4x4(&m_OffSetMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

public:
	void Set_Parent(CBone* pParent) {
		m_pParent = pParent;
		Safe_AddRef(m_pParent);
	}

public:
	HRESULT Initialize(aiNode* pAINode, class CModel* pModel, CBone* pParent);
	void SetUp_TransformationMatrix(_fmatrix Matrix);
	void SetUp_CombinedTransformationMatrix();
	void SetUp_OffsetMatrix(_fmatrix Matrix);

private:
	char			m_szName[MAX_PATH] = "";
	CBone*			m_pParent = { nullptr };
	_int			m_iParentIndex = { -1 };
	_float4x4		m_OffSetMatrix;
	_float4x4		m_TransformationMatrix; /* 뼈의 상태행렬 : 부모기준으로 표현된 이 뼈만의 상태변환행렬. */
	_float4x4		m_CombinedTransformationMatrix; /* 뼈의 상태행렬 : m_TransformationMatrix * 부모`s  m_CombinedTransformationMatrix*/

public:
	static CBone* Create(aiNode* pAINode, CModel* pModel, CBone* pParent);
	CBone* Clone();
	virtual void Free() override;
};

END