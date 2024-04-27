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

protected: // Animation
	struct ANIM_INFO
	{
		float fAnimTime = { 1.f };
		int iStartIndex = { 0 };
		int iEndIndex = { 0 };
	};
	ANIM_INFO* m_pAnimInfo = { nullptr };
	_uint m_iCurrentAnim = { 0 };
	float m_fTimeAcc = { 0.f };

	void Change_TextureSize()
	{
		const _float2 fSize = m_pTextureCom->Get_OriginalTextureSize(m_tSpriteInfo.iTextureIndex);
		m_pTransformCom->Set_Scaled(_float3(fSize.x * m_tSpriteInfo.fSizeRatio.x, fSize.y * m_tSpriteInfo.fSizeRatio.y, 1.f));
	};

	virtual void Add_Animation() = 0;
	virtual void Play_Animation(_double TimeDelta);

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr);
	//class CComponent* Find_Component(const _tchar* pComponentTag);
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
	_uint	m_iShaderPassIndex = { 0 };
	Engine::tSpriteInfo m_tSpriteInfo;
	// m_WorldMatrix�� CTransform���� ��� ���̱⿡ ���� ����ϸ� �ȵȴ�.
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