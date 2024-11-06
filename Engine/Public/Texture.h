#pragma once

#include "Component.h"

/* 경로에 해당하는 텍스쳐들을 로드하여 저장한다. */
/* 저장하고 있는 텍스쳐 중, 지정한 index의 텍스쳐를 셰이더 전역으로 던진다. */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	HRESULT Set_ShaderResource(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Set_ShaderResource(class CShader* pShader, const char* pConstantName, const _tchar* pFileName);
	HRESULT Set_ShaderResourceArray(class CShader* pShader, const char* pConstantName);
public:
	/** 폴더 경로 받으면 폴더 안 이미지 경로 모두 보관 */
	virtual HRESULT Initialize_Prototype(const vector<TCHAR*>& TextureFileVec);
	/** 텍스처 경로 직접 모두 명시 */
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	inline void Set_OriginalTextureSize(const _uint iTextureIndex, const _float2 fSize)
	{
		if (m_iNumTextures <= iTextureIndex || 0 > iTextureIndex)
		{
			MSG_BOX("CTexture - Set_TextureSize() - Stack Index is Wrong");
		};

		m_TextureSizeVec[iTextureIndex] = fSize;
	};

	const _float2 Get_OriginalTextureSize(const _uint iTextureIndex) const
	{
		if (m_iNumTextures <= iTextureIndex || 0 > iTextureIndex)
		{
			MSG_BOX("CTexture - Get_OriginalTextureSize() - Stack Index is Wrong");
			return _float2(0.f, 0.f);
		};

		return m_TextureSizeVec[iTextureIndex];
	}

	const _float2 Get_OriginalTextureSize(const _tchar* pFileName) const
	{
		if (nullptr == pFileName)
		{
			MSG_BOX("CTexture - Get_OriginalTextureSize() - NULL");
			return _float2(0.f, 0.f);
		};

		auto iter = find_if(m_SRVMap.begin(), m_SRVMap.end(), CTag_Finder(pFileName));
		if (m_SRVMap.end() == iter)
		{
			MSG_BOX("CTexture - Get_OriginalTextureSize() - NULL");
			return _float2(0.f, 0.f);
		}

		return iter->second.second;
	}

	const vector<const _tchar*>* Get_TexturePathVec() const
	{
		return &m_TexturePathVec;
	}

	const _uint& Get_TextureNum() const
	{
		return m_iNumTextures;
	}

	const _uint& Get_TextureIndex() const
	{
		return m_iTextureIndex;
	}

	const _uint& Get_Order() const
	{
		return m_iOrder;
	}

	virtual const HRESULT Get_ComponentInfo(COMPONENT_INFO& tComponentInfo) const override
	{
		tComponentInfo.pPrototypeTag = m_pPrototypeTag;
		return S_OK;
	}

private:
	const _float2 Get_OriginalTextureSize(ID3D11ShaderResourceView* pSRV) const;

private:
	vector<ID3D11ShaderResourceView*>			m_SRVs;
	unordered_map<const _tchar*, pair<ID3D11ShaderResourceView*, _float2>>			m_SRVMap; // <텍스처명, pair<리소스, 텍스처 사이즈>
	vector<const _tchar*>						m_TexturePathVec;
	vector<_float2>								m_TextureSizeVec; // 인덱스로 텍스처의 원본 크기를 얻기 위함.
	_uint										m_iNumTextures = { 0 };
	_uint										m_iTextureIndex = { 0 };
	_uint										m_iOrder = { 0 };

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_tchar*>& pFilePathVec);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END