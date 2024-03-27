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
	Assimp::Importer				m_Importer; /* ����� ������ �о �����ϴ� ����� �Ѵ�. */
	const aiScene*					m_pAIScene = { nullptr }; /* ���� �����͵��� �����ϰ� ������ ������ ���� ���� �����ϵ��� ó�����ִ� ��ü. */

private:
	TYPE					m_eType = { TYPE_END };

private:	
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	typedef vector<class CMesh*>	MESHES;

private:
	_float4x4				m_PivotMatrix;

private:
	/* �𵨿��� ���ǵǾ��ִ� ���׸����� ����. */
	_uint					m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>	m_Materials;

private:
	/* �� �ϳ��� ���� ����ϴ� ��� ����. */
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

