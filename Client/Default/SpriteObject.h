#pragma once
#include "Client_Defines.h"
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
class CState;

// @qurious. 분리해주는 이유가 뭐지? 엔진과 클라 간의 충돌 방지?
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CWidget;
END

BEGIN(Client)

/* 순수 가상 함수 */
class CSpriteObject abstract : public CGameObject
{
public:
	explicit CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSpriteObject() = default;

public:
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

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
	_uint m_iAnimTypeNum = { 0 };
	_uint m_iAnimType = { 0 };
	_double m_AnimAcc = { 0.f };

	virtual void Add_Animation() = 0;
	virtual void Play_Animation(_double TimeDelta, _uint& iSpriteIndex, const _uint iAnimType = 0);
	virtual void End_Animation(_uint& iSpriteIndex);
	
	void	Switch_SpriteDirection()
	{
		m_eSpriteDirection = (m_eSpriteDirection == SPRITE_DIRECTION::LEFT ? SPRITE_DIRECTION::RIGHT : SPRITE_DIRECTION::LEFT);
		if (m_bIsAnimUV)
		{
			// 적 이미지 디폴트 방향이 오른쪽임 (Flip = 왼쪽 보는 상태)
			m_iShaderPassIndex = ((m_eSpriteDirection == SPRITE_DIRECTION::LEFT ? (_uint)VTXTEX_PASS::FlipUV_Anim : (_uint)VTXTEX_PASS::UV_Anim));
		}
		// AnimUV가 아닐 경우의 Filp 버전은 현재 없음	
	}

public:
	virtual void	Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection = SPRITE_DIRECTION::DIRECTION_END);
	HRESULT			Change_TextureComponent(const _tchar* pPrototypeTag);

public:
	const int		Get_Attack()
	{
		return m_iMagicAttack;
	}
	void			Set_Damaged(const int iAttack)
	{
		m_iHp -= (iAttack - m_iDefense);
	}

public:
	void Set_ShaderPass(const _uint iPassIndex)
	{
		m_iShaderPassIndex = iPassIndex;
	}

	void Set_SpriteDirection(const SPRITE_DIRECTION eDirection)
	{
		m_eSpriteDirection = eDirection;
	}

public:
	const bool IsEndSprite()
	{
		bool bIsSprite = m_bIsEndSprite;
		m_bIsEndSprite = false;
		return bIsSprite;
	}

	const SPRITE_DIRECTION& Get_SpriteDirection() const
	{
		return m_eSpriteDirection;
	}

	const STATE_TYPE& Get_CurrentState() const
	{
		return m_eCurrentState;
	}

	//CState* Get_CurrentStatePointer() const
	//{
	//	return m_pState;
	//}

	void Set_SpriteInfo(const SPRITE_INFO tSpriteInfo)
	{
		m_tSpriteInfo = tSpriteInfo;
	}

	void Set_CurrentState(const STATE_TYPE tState)
	{
		m_eCurrentState = tState;
	}

	const SPRITE_INFO& Get_SpriteInfo() const
	{
		return m_tSpriteInfo;
	}

	void	Set_SpriteFileName(const _tchar* pTextureTag)
	{
		if (nullptr == pTextureTag)
			return;

		//Safe_Delete_Array(m_pSpriteFileName);
		m_pSpriteFileName = pTextureTag;
	}

public: 
	inline void CSpriteObject::Change_AnimType(const _uint& iAnimType)
	{
		if (m_iAnimTypeNum <= iAnimType)
			return;

		m_iAnimType = iAnimType;
		const ANIM_INFO* pAnim = m_pAnimInfo + m_iAnimType;
		if (m_bIsAnimUV)
		{
			m_iUVTextureIndex = pAnim->iStartIndex;
		}
		else
		{
			m_iTextureIndex = pAnim->iStartIndex;
		}
	}

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr);
	//class CComponent* Find_Component(const _tchar* pComponentTag);

	virtual HRESULT SetUp_ShaderResources() = 0;
	HRESULT SetUp_ShaderDefault();
	HRESULT SetUp_Shader_UVAnim();
	HRESULT SetUp_Shader_Wrap();

	HRESULT Load_Components_Excel();
	HRESULT	Attach_Collider(const _tchar* pLayerTag, CCollider* pCollider);
	_vector Adjust_PositionUp_Radius(const _float& RadiusY);
	void	MoveToDirection(const SPRITE_DIRECTION& Direction, _double TimeDelta);

private:
	HRESULT CSpriteObject::Mapping_Component(const _tchar* pComponentTag);

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CWidget* m_pWidgetCom = { nullptr };

protected:
	bool	m_bIsAnimUV;
	bool	m_bIsEndSprite; 

	_vector					m_vColor = { 1.f, 1.f, 1.f, 1.f };

	CState*					m_pState;
	CState*					m_pAirState;
	_bool					m_bIsInAir;

	STATE_TYPE				m_eCurrentState;
	SPRITE_DIRECTION		m_eSpriteDirection;
	_uint	m_iUVTextureIndex;
	// UV용 텍스처의 가로 텍스처 개수
	_uint	m_iUVTexNumX;
	// UV용 텍스처의 세로 텍스처 개수
	_uint	m_iUVTexNumY;

	const		_tchar* m_pSpriteFileName = { nullptr };
	/** @note - 템플릿 변수는 static으로 선언해야 한다. - static은 무조건 외부 초기화 */

protected:
	SPRITE_INFO m_tSpriteInfo;

	SKUL_RANK	m_eSkulRank;
	SKUL_TYPE	m_eSkulType;

	_uint	m_iLevel = { 1 };

	_int m_iHp = { 100 };
	_int m_iMagicAttack = { 10 };
	_int m_iPhysicalAttack = { 10 };
	_int m_iDefense = { 1 };
	_int m_iMaxJumpCount = { 2 };

	_float m_fMovementSpeed = { 10 };
	_float m_fAttackSpeed = { 10 };
	_float m_fReduceCoolDownSpeed = { 0 };
	_float m_fCriticalHit;

	_int m_iMagicAttackIncrease = { 0 };
	_int m_iPhysicalAttackIncrease = { 0 };
	_int m_iDefenseIncrease = { 0 };

	_double m_InvulnerabilityDuration = { 0.0 };

public:
	virtual CGameObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;
};


END
