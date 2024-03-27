#pragma once

#include "Model.h"
#include "VIBuffer.h"

/* 모델을 구성하는 하나의 메시. 메시의 교체를 노리기위해. */
/* 메시별로 정점, 인덱스버퍼를 가진다. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void Get_BoneMatrices(_Out_ _float4x4* pBoneMatrices, _In_ const vector<class CBone*>& Bones, _In_ _matrix PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	char		m_szName[MAX_PATH] = "";
	_uint		m_iMaterialIndex = { 0 };

private:
	_uint					m_iNumBones = { 0 };
	vector<_uint>			m_Bones;	

private:
	HRESULT Ready_VertexBuffer_ForNonAnim(aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_VertexBuffer_ForAnim(aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END