#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	class CBone* Get_BonePtr(const char* pBoneName);
	_uint Get_BoneIndex(const char* pBoneName);

	_matrix Get_PivotMatrix() const {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

public:
	void Set_Animation(_uint iIndex) {
		m_iCurrentAnimationIndex = iIndex;
	}


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_ShaderResource(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT SetUp_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	void Play_Animation(_double TimeDelta);

	HRESULT Render(_uint iMeshIndex);

private:
	Assimp::Importer				m_Importer; /* 경로의 파일을 읽어서 저장하는 기능을 한다. */
	const aiScene*					m_pAIScene = { nullptr }; /* 실제 데이터들을 저장하고 있으며 변수를 통해 접근 가능하도록 처리해주는 객체. */

private:
	TYPE					m_eType = { TYPE_END };

private:	
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	typedef vector<class CMesh*>	MESHES;

private:
	_float4x4				m_PivotMatrix;

private:
	/* 모델에게 정의되어있는 머테리얼의 개수. */
	_uint					m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>	m_Materials;

private:
	/* 이 하나의 모델이 사용하는 모든 뼈들. */
	vector<class CBone*>		m_Bones;

private:
	_uint						m_iCurrentAnimationIndex = { 0 };
	_uint						m_iNumAnimations = { 0 };
	vector<class CAnimation*>	m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, CBone* pParent);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

