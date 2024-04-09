#pragma once

#include "Client_Defines.h"
#include "SpriteObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CBackGround final : public CSpriteObject
{
private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* ������ü�� �ʱ�ȭ�۾� */
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override; /* �纻��ü�� �ʱ�ȭ�۾� */
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;	
	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� ������. */

public:
	/* Prototype */
	/* ���� ��ü�� �����Ѵ�. */
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻 ��ü�� �����Ѵ�. */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr);
	virtual void Free() override;
};

END