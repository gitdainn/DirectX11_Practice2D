#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

#pragma region Component
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#pragma endregion

/** @note - 전방선언의 기준은?
- CState와 CSpriteObject 처럼 서로 상호작용하고 있는 경우 둘 중 하나라도 먼저 정의가 완료되지 않은 상태에서 참조하므로 오류가 뜸. 이때 전방선언 필요
- 현재 CState* m_pState처럼 보유만 하고 있는 곳에서는 CState라는 애 있어~ 라고 전방선언만 해주고, 실제 구현부 -> 사용하는 곳에서 cpp 추가해주기
*/

/** @qurious - 근데 파라미터로 CSpriteObject를 받는 경우에는 전방선언 X 헤더를 추가해줘야하는듯함. (CState 클래스 참고)*/
// @qurious. 분리해주는 이유가 뭐지? 엔진과 클라 간의 충돌 방지?
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Tool)

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

protected: // Animation
	struct ANIM_INFO
	{
		_float fAnimTime = { 0.f };
		// @qurious - vector를 하려면 딜레이없는 애들까지 공간 다 만들어줘야하고, 공간을 아끼자니 몇번째 인덱스인지 .find()를 써야함 ㅁ가 더 좋을까?
		/** @qurious - map 타입 정의에는 const 쓰면 안됨 */
		unordered_map<_uint, _float> fDelayTimeMap;
		_uint iStartIndex = { 0 };
		_uint iEndIndex = { 0 };
	};
	ANIM_INFO* m_pAnimInfo = { nullptr };
	_uint m_iCurrentAnim = { 0 };
	float m_fTimeAcc = { 0.f };

	//virtual void Add_Animation();
	virtual void Play_Animation(_uint& iSpriteIndex, _double TimeDelta);

public:
	void Set_ShaderPass(const _uint iPassIndex)
	{
		m_iShaderPassIndex = iPassIndex;
	}

	void Set_SpriteDirection(const SPRITE_DIRECTION eDirection)
	{
		m_eSpriteDirection = eDirection;
	}

	void Set_SpriteInfo(const SPRITE_INFO tSpriteInfo)
	{
		m_tSpriteInfo = tSpriteInfo;
	}

	void Set_SpriteTag(const char* pTag)
	{
		Safe_Delete_Array(m_pSpriteTag);
		m_pSpriteTag = pTag;
	}

	void Set_ScaleRatio(const _float2& fSizeRatio)
	{
		m_tSpriteInfo.fSizeRatio = fSizeRatio;
		_float2 fOriginalSize = m_pTextureCom->Get_OriginalTextureSize(m_tSpriteInfo.iTextureIndex);
		fOriginalSize.x *= m_tSpriteInfo.fSizeRatio.x;
		fOriginalSize.y *= m_tSpriteInfo.fSizeRatio.y;
		m_tSpriteInfo.fSize = fOriginalSize;
		m_pTransformCom->Set_Scaled(_float3(fOriginalSize.x, fOriginalSize.y, 1.f));
	}

	void Set_IsScroll(const _bool& bIsScroll)
	{
		m_bIsScroll = bIsScroll;
	};

public:
	const bool IsEndSprite()
	{
		bool bIsSprite = m_bIsEndSprite;
		m_bIsEndSprite = false;
		return bIsSprite;
	}

	const SPRITE_INFO& Get_SpriteInfo() const
	{
		return m_tSpriteInfo;
	}

	const SPRITE_DIRECTION& Get_SpriteDirection() const
	{
		return m_eSpriteDirection;
	}

	const vector<const _tchar*>* Get_TexturePathVec() const
	{
		return m_pTextureCom->Get_TexturePathVec();
	}

	const char* Get_SpriteTag() const
	{
		return m_pSpriteTag;
	}

	const _float2& Get_ScaleRatio() const
	{
		return m_tSpriteInfo.fSizeRatio;
	}

public:
	template<typename T>
	void Change_Sprite(const T& Sprite);
	HRESULT Change_TextureComponent(const _tchar* pPrototypeTag);

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr);
	//class CComponent* Find_Component(const _tchar* pComponentTag);
	virtual HRESULT SetUp_ShaderResources();
	void	Scroll_Screen(_float4x4& WorldMatrix) const;

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
	// m_WorldMatrix는 CTransform에서 사용 중이기에 따로 사용하면 안된다.
	_float4x4	m_ViewMatrix;
	_float4x4	m_ProjMatrix;

protected:
	_bool	m_bIsDead;
	_bool	m_bIsRender;
	_bool	m_bIsAnimUV;
	_bool	m_bIsEndSprite;
	_bool	m_bIsScroll;

	const char*		m_pSpriteTag;
	const _tchar*	m_pTextureComTag;
	SPRITE_DIRECTION		m_eSpriteDirection;
	CRenderer::RENDERGROUP	m_eRenderGroup;
	_uint	m_iUVTextureIndex;
	_uint	m_iUVTexNumX;
	_uint	m_iUVTexNumY;

	/** @note - 템플릿 변수는 static으로 선언해야 한다. - static은 무조건 외부 초기화 */

public:
	virtual CGameObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;
};

template<typename T>
inline void CSpriteObject::Change_Sprite(const T& Sprite)
{
	if (m_iCurrentAnim == (_uint)Sprite)
		return;

	m_iCurrentAnim = (_uint)Sprite;
	if (m_bIsAnimUV)
	{
		m_iUVTextureIndex = m_pAnimInfo[(_uint)Sprite].iStartIndex;
	}
	else
	{
		m_tSpriteInfo.iTextureIndex = m_pAnimInfo[(_uint)Sprite].iStartIndex;
	}
}

END
