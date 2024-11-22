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
class CCollider;
END

BEGIN(Tool)

/* 순수 가상 함수 */
class CSpriteObject : public CGameObject
{
public:
	explicit CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSpriteObject() = default;

public:
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr) override;
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
	_uint m_iAnimType = { 0 };
	_double m_AnimAcc = { 0.f };

	//virtual void Add_Animation();
	virtual void Play_Animation(_double TimeDelta, _uint& iSpriteIndex, const _uint& iAnimType = 0);
	HRESULT Load_Components_Excel();

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

public:
	void	Set_TextureIndex(const _uint iIndex)
	{
		m_iTextureIndex = iIndex;
	}

	void Set_SpriteDirection(const SPRITE_DIRECTION& eDirection)
	{
		m_eSpriteDirection = eDirection;
	}

	void Set_SpriteInfo(const SPRITE_INFO& tSpriteInfo)
	{
		m_tSpriteInfo = tSpriteInfo;
	}

	void Set_SpriteTag(const char* pTag)
	{
		Safe_Delete_Array(m_pSpriteTag);
		m_pSpriteTag = pTag;
	}

	void Set_PrototypeTag(const _tchar* pTag)
	{
		Safe_Delete_Array(m_tSpriteInfo.pPrototypeTag);
		m_tSpriteInfo.pPrototypeTag = pTag;
	}

	/** @notice - 크기 비율 적용 시 m_tSpriteInfo의 크기를 변경하지 말고 아래 함수를 사용해주세요. 
	* 원본 크기가 들어있는 m_tSpriteInfo.fSize를 변경하면 비율이 누적돼서 거대해지는 문제가 발생합니다. */
	void Set_ScaleRatio(const _float2 fSizeRatio)
	{
		m_tSpriteInfo.fSizeRatio = fSizeRatio;
		if (!m_bIsAnimUV)
		{
			m_pTransformCom->Set_Scaled(_float3(m_tSpriteInfo.fSize.x * m_tSpriteInfo.fSizeRatio.x
				, m_tSpriteInfo.fSize.y * m_tSpriteInfo.fSizeRatio.y, 1.f));
		}
		else
		{
			_float3 vScale = _float3(m_tSpriteInfo.fSize.x, m_tSpriteInfo.fSize.y, 0.f);
			vScale.x /= m_iUVTexNumX;
			vScale.y /= m_iUVTexNumY;
			vScale.x *= fSizeRatio.x;
			vScale.y *= fSizeRatio.y;

			m_pTransformCom->Set_Scaled(vScale);
		}

	}

	void Set_IsScroll(const _bool bIsScroll) { 
		m_bIsScroll = bIsScroll;
	};

	void	Set_IsAnimUV(const _bool bIsAnim) { m_bIsAnimUV = bIsAnim; }
	void	Set_UVTexNumX(const _uint iUVTexNum) { m_iUVTexNumX = iUVTexNum; }
	void	Set_UVTexNumY(const _uint iUVTexNum) { m_iUVTexNumY = iUVTexNum; }
	void	Set_SliceTextureSize(const _uint iUVTexNumX, const _uint iUVTexNumY)
	{
		_float3 vScale = _float3(m_tSpriteInfo.fSize.x, m_tSpriteInfo.fSize.y, 0.f);
		vScale.x /= iUVTexNumX;
		vScale.y /= iUVTexNumY;

		m_pTransformCom->Set_Scaled(vScale);
	}

	void	Set_SpriteFileName(const _tchar* pTextureTag)
	{
		if (nullptr == pTextureTag)
			return;

		m_pSpriteFileName = pTextureTag;
	}
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

	const _float2 Get_ScaleRatio() const
	{
		return m_tSpriteInfo.fSizeRatio;
	}

	const CRenderer::RENDERGROUP& Get_RenderGroup() const
	{
		return m_eRenderGroup;
	}

	const _tchar* Get_SpriteFileName() const { return m_pSpriteFileName; }

	const	_bool	Get_bIsAnimUV() const { return m_bIsAnimUV; }
	const	_uint	Get_UVTexNumX() const { return m_iUVTexNumX; }
	const	_uint	Get_UVTexNumY() const { return m_iUVTexNumY; }

public:
	template<typename T>
	void Change_AnimType(const T& Sprite);
	HRESULT Change_TextureComponent(const _tchar* pPrototypeTag);
	HRESULT Mapping_Component(const _tchar* pComponentTag);

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr);
	//class CComponent* Find_Component(const _tchar* pComponentTag);
	virtual HRESULT SetUp_ShaderResources() = 0;
	HRESULT SetUp_Shader_Camera();
	HRESULT SetUp_Shader_UVAnim();
	HRESULT SetUp_Shader_Wrap();

	void	Scroll_Screen(_float4x4& WorldMatrix) const;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

protected:
	SPRITE_INFO m_tSpriteInfo;

	_bool	m_bIsOnce = { false };
	_bool	m_bIsAnimUV;
	_bool	m_bIsEndSprite;

	const char*		m_pSpriteTag;
	const _tchar*		m_pTextureComTag;
	SPRITE_DIRECTION		m_eSpriteDirection;
	
	_uint	m_iUVTextureIndex = { 0 };
	_uint	m_iUVTexNumX = { 1 };
	_uint	m_iUVTexNumY = { 1 };

	const		_tchar* m_pSpriteFileName = { nullptr };
	/** @note - 템플릿 변수는 static으로 선언해야 한다. - static은 무조건 외부 초기화 */

public:
	virtual CGameObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;
};

template<typename T>
inline void CSpriteObject::Change_AnimType(const T& Sprite)
{
	if (m_iAnimType == (_uint)Sprite)
		return;

	m_iAnimType = (_uint)Sprite;
	if (m_bIsAnimUV)
	{
		m_iUVTextureIndex = m_pAnimInfo[(_uint)Sprite].iStartIndex;
	}
	else
	{
		m_iTextureIndex = m_pAnimInfo[(_uint)Sprite].iStartIndex;
	}
}

END
