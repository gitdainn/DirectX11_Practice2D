#include "..\Public\Model.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eType(rhs.m_eType)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		CBone*		pBone = pPrototypeBone->Clone();
		if (nullptr == pBone)
			return;
		m_Bones.push_back(pBone);
	}

	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		CAnimation*	pAnimation = pPrototypeAnimation->Clone();
		if (nullptr == pAnimation)
			return;

		m_Animations.push_back(pAnimation);
	}	

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_AddRef(pTexture);
	}
		
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		if (false == strcmp(pBoneName, pBone->Get_Name()))
			return true;

		return false;
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;	
}

_uint CModel::Get_BoneIndex(const char * pBoneName)
{	
	_uint		iIndex = 0;

	for (auto& pBone : m_Bones)
	{
		if (false == strcmp(pBone->Get_Name(), pBoneName))		
			break;

		++iIndex;
	}

	return iIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	_uint		iFlag = 0;

	m_eType = eType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	/* aiProcess_PreTransformVertices : 정점정보를 읽어서 저장할 당시에 필요한 변환을 미리 처리해놓는다. */
	/* 처리? 이 모델의 뼈들 중, 메시의 이름과 같은 이름을 가진 뼈를 찾아서 그 뼈의 행렬을 정점들에게 미리 적용한다. */
	if(TYPE_NONANIM == m_eType)
		iFlag = aiProcess_PreTransformVertices | aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
	else
		iFlag = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
 
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;

	/* 모델을 구성하는 메시들을 생성한다.(버텍스, 인덱스버퍼를 생성하는 것이다.) */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	for (auto& pBone : m_Bones)
	{
		_int		iIndex = pBone->Get_ParentIndex();

		if(0 <= iIndex)
			pBone->Set_Parent(m_Bones[iIndex]);
	}

	return S_OK;
}

HRESULT CModel::SetUp_ShaderResource(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if(nullptr != m_Materials[iMaterialIndex].pMtrlTexture[eType])
	{
		if (FAILED(m_Materials[iMaterialIndex].pMtrlTexture[eType]->Set_ShaderResource(pShader, pConstantName)))
			return E_FAIL;
	}

	return S_OK;
}

/* 모델을 렌더링할때 모델의 정점들이 따라가야할 뼈들의 행렬을 배열로만들어 셰이더로 던진다. */
HRESULT CModel::SetUp_BoneMatrices(CShader * pShader, const char * pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));

	return pShader->Set_MatrixArray(pConstantName, BoneMatrices, 256);	
}

void CModel::Play_Animation(_double TimeDelta)
{
	/* 실제 파일에 저장되어있었던 뼈의 각각의 시간에 맞는 상태로 뼈들의 상태를 변경하고. 
	이렇게 변경된 뼈들의 상태를 부모에서부터 자식으로 순차적으로 누적하여 셋팅해나간다. */
	/*모든 뼈의 상태를 갱신한다.*/

	/* CAnimation : 특정 동작을 표현하는 객체. */
	/* : 이 동작을 구현하기위해 필요한 뼈들의 상태정보(시간에따라 다수, m_TransformationMatrix)를 가진다. */
	m_Animations[m_iCurrentAnimationIndex]->Update(m_Bones, TimeDelta);

	/* 최상위 부모뼈부터 시작하여 최하위 자식뼈까지 전체를 순회하며 Combine행렬을 다시 만들어낸다. */
	for (auto& pBone : m_Bones)
	{
		pBone->SetUp_CombinedTransformationMatrix();
	}
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (nullptr != m_Meshes[iMeshIndex])
		m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}


	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	/* 머테리얼을 표현하기위한 텍스쳐를 로드하자. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];
		MESH_MATERIAL	ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof ModelMaterial);

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint	iNumTextures = pAIMaterial->GetTextureCount(aiTextureType(j));
		
			aiString		TexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFileName[MAX_PATH] = "";
			char		szEXT[MAX_PATH] = "";

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			char		szDrive[MAX_PATH] = "";
			char		szDir[MAX_PATH] = "";

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar		szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szFinalPath, MAX_PATH);

			ModelMaterial.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath);
			if (nullptr == ModelMaterial.pMtrlTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, CBone* pParent)
{
	CBone*		pBone = CBone::Create(pAINode, this, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], pBone);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel *	pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel *	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();


	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMtrlTexture)
			Safe_Release(pTexture);
	}

	m_Materials.clear();

	m_Importer.FreeScene();

}
