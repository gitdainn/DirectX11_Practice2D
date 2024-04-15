#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

#pragma region Component
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#pragma endregion

// @qurious. �и����ִ� ������ ����? ������ Ŭ�� ���� �浹 ����?
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

/* ���� ���� �Լ� */
class CSpriteObject : public CGameObject
{
public:
	explicit CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSpriteObject() = default;

public:
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Add_Components(void* pArg = nullptr);
	class CComponent* Find_Component(const _tchar* pComponentTag);
	virtual HRESULT SetUp_ShaderResources();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };


protected:
	/* �ؽ����̺� */
	unordered_map<const _tchar*, class CComponent*>			m_Components;
	int	m_iShaderPassIndex;
	Engine::tSpriteInfo m_tSpriteInfo;
	_float4x4	m_ViewMatrix;
	_float4x4	m_ProjMatrix;

protected:
	bool	m_bIsDead;
	bool	m_bIsRender;
	CRenderer::RENDERGROUP	m_eRenderGroup;

public:
	virtual CGameObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END