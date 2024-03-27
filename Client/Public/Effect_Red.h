#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Point_Instance;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CEffect_Red final : public CGameObject
{
private:
	CEffect_Red(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Red(const CEffect_Red& rhs);
	virtual ~CEffect_Red() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* ������ü�� �ʱ�ȭ�۾� */
	virtual HRESULT Initialize(void* pArg) override; /* �纻��ü�� �ʱ�ȭ�۾� */
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform*					m_pTransformCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� ������. */

public:
	/* Prototype */
	/* ���� ��ü�� �����Ѵ�. */
	static CEffect_Red* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻 ��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END