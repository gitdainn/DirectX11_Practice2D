#pragma once

#include "Component.h"

/* ��ο� �ش��ϴ� �ؽ��ĵ��� �ε��Ͽ� �����Ѵ�. */
/* �����ϰ� �ִ� �ؽ��� ��, ������ index�� �ؽ��ĸ� ���̴� �������� ������. */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	HRESULT Set_ShaderResource(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Set_ShaderResourceArray(class CShader* pShader, const char* pConstantName);
public:
	/** ���� ��� ������ ���� �� �̹��� ��� ��� ���� */
	virtual HRESULT Initialize_Prototype(const vector<TCHAR*>& TextureFileVec);
	/** �ؽ�ó ��� ���� ��� ��� */
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

private:
	vector<ID3D11ShaderResourceView*>			m_SRVs;
	_uint										m_iNumTextures = { 0 };

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<_tchar*>& pFilePathVec);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END