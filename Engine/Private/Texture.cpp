#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

/** @qurious - 사본 생성 시 clone해서 생성자 불릴 때, 복사 생성자 자동 호출돼서 기본 자료형은 1대1 대입되지 않나?
* vector인 m_SRVs는 Safe_AddRef까지 직접 명시해줘야한다해도 m_INumTextures도 굳이 명시 해줘야 하는가? 
-> 선언을 안했으면 기본 대입인데 명시 선언을 했으니까 지본 자료형까지 모두 직접 적어서 해줘야하는건가? */
CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
	, m_TextureSizeVec(rhs.m_TextureSizeVec)
{
	for (ID3D11ShaderResourceView* pSRV : m_SRVs)
		Safe_AddRef(pSRV);

	_uint iIndex = 0;
	m_TexturePathVec.resize(rhs.m_TexturePathVec.size());
	for (const _tchar* pFilePath : rhs.m_TexturePathVec)
	{
		_tchar* pPath = new _tchar[lstrlen(pFilePath) + 1];
		lstrcpy(pPath, pFilePath);
		m_TexturePathVec[iIndex] = pPath;
		++iIndex;
	}
}

HRESULT CTexture::Set_ShaderResource(CShader * pShader, const char * pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Set_ShaderResourceView(pConstantName, m_SRVs[iTextureIndex]);	
}

HRESULT CTexture::Set_ShaderResourceArray(CShader * pShader, const char * pConstantName)
{
	ID3D11ShaderResourceView*		pSRVs[2] = {
		m_SRVs[0], 
		m_SRVs[1],
	};
	return pShader->Set_ShaderResourceViewArray(pConstantName, pSRVs, m_iNumTextures);	
}

HRESULT CTexture::Initialize_Prototype(const vector<TCHAR*>& TextureFileVec)
{
	m_iNumTextures = (_uint)TextureFileVec.size();
	/** @qurious - resize와 reverse 차이, 그리고 emplace_back */
	m_TextureSizeVec.resize(m_iNumTextures);
	m_TexturePathVec.resize(m_iNumTextures);

	_uint iIndex = { 0 };

	for (const _tchar* pFilePath : TextureFileVec)
	{
		_tchar		szDrive[MAX_PATH] = TEXT("");
		_tchar		szDir[MAX_PATH] = TEXT("");
		_tchar		szFileName[MAX_PATH] = TEXT("");
		_tchar		szEXT[MAX_PATH] = TEXT("");

		_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT		hr = 0;

		if (!lstrcmp(szEXT, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, pFilePath, nullptr, &pSRV);
		}
		else if (!lstrcmp(szEXT, TEXT(".tga")))
			return E_FAIL;
		else
			hr = CreateWICTextureFromFile(m_pDevice, pFilePath, nullptr, &pSRV);

		m_SRVs.emplace_back(pSRV);
		m_TextureSizeVec[iIndex] = (Get_OriginalTextureSize(pSRV));

		_tchar* pPath = new _tchar[lstrlen(pFilePath) + 1];
		lstrcpy(pPath, pFilePath);

		m_TexturePathVec[iIndex] = pPath;
		++iIndex;
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;
	// 텍스처 개수 미리 알고있어 메모리 재할당 문제 방지 가능하고, 검색이 중요하기에 벡터 선정. 
	m_TextureSizeVec.resize(m_iNumTextures);
	m_TexturePathVec.resize(m_iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		// wsprintf: 2번째 인자에 %d로 되어 있는 부분이 있으면 3번째 인자값으로 대체해서 첫번째 인자에 저장하는 함수
		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar		szEXT[MAX_PATH] = TEXT("");
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		ID3D11ShaderResourceView*	pSRV = { nullptr };

		HRESULT		hr = 0;
		if (!lstrcmp(szEXT, TEXT(".tga")))
		{
			return E_FAIL;
		}
		else if (!lstrcmp(szEXT, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		}

		if (FAILED(hr))
		{
			MSG_BOX("CTexture - Initialize_Prototype() - Failed Create File");
			return E_FAIL;
		}

		m_SRVs.emplace_back(pSRV);
		m_TextureSizeVec[i] = (Get_OriginalTextureSize(pSRV));

		_tchar* pPath = new _tchar[lstrlen(szFullPath) + 1];
		lstrcpy(pPath, szFullPath);

		m_TexturePathVec[i] = pPath;
	}	

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	// @note - 엑셀에 모든 컴포너느 정보를 동일한 구조체에 담은 후, 통째로 넘겨받아 자신의 정보만 골라 받는 형식으로 사용 중
	// @refactoring - Texture 컴포넌트가 TextureIndex와 Order을 관리해버리므로 Owner가 외부에서 다시 Get 해가야하는 문제 발생
	if (nullptr != pArg)
	{
		COMPONENT_INFO tComponentInfo;
		memcpy(&tComponentInfo, pArg, sizeof(COMPONENT_INFO));
		m_pPrototypeTag = tComponentInfo.pPrototypeTag;
		m_iTextureIndex = tComponentInfo.iTextureIndex;
		m_iOrder = tComponentInfo.iOrder;
	}
	return S_OK;
}

const _float2 CTexture::Get_OriginalTextureSize(ID3D11ShaderResourceView* pSRV) const
{
	//D3D11_SHADER_RESOURCE_VIEW_DESC TextureDesc;

	// 1. ID3D11ShaderResourceView로부터 텍스처를 가져옴.
	ID3D11Resource* pResource = { nullptr };
	pSRV->GetResource(&pResource);
	
	// 2. 가져온 텍스처를 ID3D11Texture2D로 캐스팅함.
	ID3D11Texture2D* pTexture = { nullptr };
	HRESULT hr = pResource->QueryInterface<ID3D11Texture2D>(&pTexture);

	if (FAILED(hr))
	{
		MSG_BOX("CTexture - Get_TextureSize() - FAILED");
		return _float2(0.f, 0.f);
	}

	D3D11_TEXTURE2D_DESC TextureDesc;
	pTexture->GetDesc(&TextureDesc);

	Safe_Release(pTexture);
	Safe_Release(pResource);

	return _float2((float)TextureDesc.Width, (float)TextureDesc.Height);
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture *	pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_tchar*>& pFilePathVec)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePathVec)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture *	pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

// @qurious - 기본 자료형 vector도 clear 해줘야하나?
void CTexture::Free()
{
	__super::Free();

	for (ID3D11ShaderResourceView* pSRV : m_SRVs)
		Safe_Release(pSRV);
	m_SRVs.clear();
	
	for (const _tchar* pPath : m_TexturePathVec)
	{
		Safe_Delete_Array(pPath);
	}
	m_TexturePathVec.clear();

	m_TextureSizeVec.clear();
}
