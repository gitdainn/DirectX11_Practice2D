#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CNavigation;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* ������ü�� �ʱ�ȭ�۾� */
	virtual HRESULT Initialize(void* pArg) override; /* �纻��ü�� �ʱ�ȭ�۾� */
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform*			m_pTransformCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTexture*			m_pTextureCom[TYPE_END] = { nullptr };		
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };



private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� ������. */

public:
	/* Prototype */
	/* ���� ��ü�� �����Ѵ�. */
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻 ��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END