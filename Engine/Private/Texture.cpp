#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
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
	for (const TCHAR* pFilePath : TextureFileVec)
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

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		// wsprintf: 2번째 인자에 %d로 되어 있는 부분이 있으면 3번째 인자값으로 대체해서 첫번째 인자에 저장하는 함수
		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar		szDrive[MAX_PATH] = TEXT("");
		_tchar		szDir[MAX_PATH] = TEXT("");
		_tchar		szFileName[MAX_PATH] = TEXT("");
		_tchar		szEXT[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		ID3D11ShaderResourceView*	pSRV = { nullptr };

		HRESULT		hr = 0;

		if (!lstrcmp(szEXT, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		else if (!lstrcmp(szEXT, TEXT(".tga")))
			return E_FAIL;
		else
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		m_SRVs.push_back(pSRV);
	}	

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
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

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

}
