#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

#pragma region Component
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "LineRider.h"
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
	explicit CSpriteObject(const CSpriteObject& rhs);
	virtual ~CSpriteObject() = default;

public:
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr);
	/** @param - 인스턴스 번호를 넘겨주세요. 데이터를 읽어들일 때 사용됩니다. */
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual HRESULT Initialize_Prototype() override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

protected: // Stats
	struct BASE_STATS
	{
		_int iMagicAttack = { 10 };
		_int iHp = { 100 };
		_int iDefense = { 1 };
		_int iPhysicalAttack = { 10 };
		_int iMaxJumpCount = { 2 };

		_float fMovementSpeed = { 10 };
		_float fAttackSpeed = { 10 };
		_float fReduceCoolDownSpeed = { 0 };
		_float fCriticalHit = { 0.f };

		_int iMagicAttackIncrease = { 0 };
		_int iPhysicalAttackIncrease = { 0 };
		_int iDefenseIncrease = { 0 };

		_bool	bIsInvulnerable = { false };
		_double InvulnerabilityDuration = { 0.0 };
	};

	virtual void	Switch_SpriteDirection()
	{
		m_eSpriteDirection = (m_eSpriteDirection == SPRITE_DIRECTION::LEFT ? SPRITE_DIRECTION::RIGHT : SPRITE_DIRECTION::LEFT);
	}

public:
	HRESULT			Change_TextureComponent(const _tchar* pPrototypeTag);

public:
	const int		Get_Attack()
	{
		return m_tBaseStats.iMagicAttack;
	}
	void			Set_Damaged(const int iAttack)
	{
		m_tBaseStats.iHp -= (iAttack - m_tBaseStats.iDefense);
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

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr);
	//class CComponent* Find_Component(const _tchar* pComponentTag);

	virtual HRESULT SetUp_ShaderResources() = 0;

	/** WVP 행렬에 관한 리소스를 넘기는 함수들입니다. 꼭 택 1하여 사용해주세요. */
	HRESULT SetUp_Shader_Camera();
	HRESULT SetUp_Shader_NonCamera(); // 카메라로 관찰되지만, 카메라의 이동에는 영향을 받지 않는 함수입니다.
	HRESULT SetUp_Shader_Orthographic();

	/** 필요에 따라 선택적으로 사용하는 부가적인 리소스 함수들입니다. */
	HRESULT SetUp_Shader_Wrap();

protected:
	/** 점프/추락 기능이 있는 객체에게 사용하는 선타기 기능입니다. */
	virtual HRESULT	JumpableLineRider(_double TImeDelta);
	/** 추락 기능이 없는 선타기 기능입니다.
	* @return - 현재 타고 있는 선을 벗어나면 E_FAIL을 반환합니다. (착지할 수 있는 땅이 없어도 E_FAIL 반환) */
	HRESULT	DefaultLineRider(const _vector vPosition, const _float fOffsetY = 0.f);

	HRESULT Load_Components_Excel();
	HRESULT	Attach_Collider(const _tchar* pLayerTag, CCollider* pCollider);
	_vector Adjust_PositionUp_Radius(const _float& RadiusY);
	void	MoveToDirection(const SPRITE_DIRECTION& Direction, _double TimeDelta);

protected:
	const _tchar*	DeepCopy(const _tchar* pTag)
	{
		_tchar* pCopy = new _tchar[lstrlen(pTag) + 1]{};
		lstrcpy(pCopy, pTag);
		return pCopy;
	}

private:
	HRESULT CSpriteObject::Mapping_Component(const _tchar* pComponentTag);
	void Scroll_Screen(_float4x4& WorldMatrix) const;

private:
	/** HLSL에 꼭 넘겨야 하는 리소스 정보입니다. */
	HRESULT	SetUp_Shader_Default();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected: // 컴포넌트 //
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CWidget* m_pWidgetCom = { nullptr };
	CLineRider* m_pLineRiderCom = { nullptr };

protected:
	bool	m_bIsAnimUV;
	bool	m_bIsEndSprite;

	_vector					m_vColor = { 1.f, 1.f, 1.f, 1.f };

	CState* m_pState = { nullptr };
	CState* m_pAirState = { nullptr };
	_bool					m_bIsInAir;

	STATE_TYPE				m_eCurrentState;
	SPRITE_DIRECTION		m_eSpriteDirection;

	const		_tchar* m_pSpriteFileName = { nullptr };
	/** @note - 템플릿 변수는 static으로 선언해야 한다. - static은 무조건 외부 초기화 */

protected:
	SPRITE_INFO m_tSpriteInfo;
	BASE_STATS	m_tBaseStats;

	SKUL_RANK	m_eSkulRank;
	SKUL_TYPE	m_eSkulType;

	_uint	m_iLevel = { 1 };

public:
	virtual CGameObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;
};


END
