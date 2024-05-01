#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

#pragma region Component
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#pragma endregion

// @qurious. 분리해주는 이유가 뭐지? 엔진과 클라 간의 충돌 방지?
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

/* 순수 가상 함수 */
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

public:
	void Set_IsMove(const CONTROL_KEY eMove, const bool bIsMove = true)
	{
		m_bIsMove = true;
		m_eMoveDir = eMove;
	}

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

	virtual void Add_Animation() = 0;
	virtual void Play_Animation(_uint& iSpriteIndex, _double TimeDelta);
	virtual void Move(_double TimeDelta);

public:
	template<typename T>
	void Change_Motion(const T& Motion);

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
	/* 해시테이블 */
	unordered_map<const _tchar*, class CComponent*>			m_Components;
	_uint	m_iShaderPassIndex = { 0 };
	Engine::tSpriteInfo m_tSpriteInfo;
	// m_WorldMatrix는 CTransform에서 사용 중이기에 따로 사용하면 안된다.
	_float4x4	m_ViewMatrix;
	_float4x4	m_ProjMatrix;

protected:
	bool	m_bIsDead;
	bool	m_bIsRender;
	bool	m_bIsMove;
	bool	m_bIsJump;
	bool	m_bIsFall;
	bool	m_bIsAnimUV;

	CONTROL_KEY				m_eMoveDir;
	CRenderer::RENDERGROUP	m_eRenderGroup;
	_uint	m_iUVTextureIndex;
	_uint	m_iUVTexNumX;
	_uint	m_iUVTexNumY;

public:
	virtual CGameObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;
};

template<typename T>
inline void CSpriteObject::Change_Motion(const T& Motion)
{
	if (m_iCurrentAnim == (_uint)Motion)
		return;
	
	m_iCurrentAnim = (_uint)Motion;
	if (m_bIsAnimUV)
	{
		m_iUVTextureIndex = m_pAnimInfo[(_uint)Motion].iStartIndex;
	}
	else
	{
		m_tSpriteInfo.iTextureIndex = m_pAnimInfo[(_uint)Motion].iStartIndex;
	}
}

END
