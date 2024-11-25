#pragma once
#include "SpriteObject.h"
#include "Player_Manager.h"
#include <functional>


class CItem abstract : public CAnimObject
{
public:
	explicit CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CItem(const CItem& rhs);
	~CItem() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Late_Initialize() override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

#pragma region 임시로 삽입했습니다. 점프 코드가 매우 중복되므로 한 곳으로 합쳐주세요.
protected:
	void Parabola(const _double TimeDelta)
	{
		const _float fGravity = 9.8f;
		const _float fJumpSpeed = 3.f;

		m_JumpTimeAcc += TimeDelta * fJumpSpeed;

		// sin(포물선운동 진행각도90) = 1
		m_UpTime = m_fPower * m_JumpTimeAcc * (_float)TimeDelta;
		m_DownTime = (fGravity * m_JumpTimeAcc * m_JumpTimeAcc) * 0.5f;

		_float fJumpY = (_float)(m_UpTime - m_DownTime);
		// @note - 전체적인 곡선 모양은 동일한데 속도만 빨리하고 싶다면 그래프상 시간을 빠르게 흐르게 하면 된다! (즉 시간만 속도 올려주기)

		if (m_UpTime < m_DownTime)
			m_bIsFalling = true;
		else
			m_bIsFalling = false;

		if (nullptr == m_pTransformCom)
			return;

		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		/** @qurious - 포물선 점프로 얻은 값을 증가되는 현재 Y위치에 더해줘야함.. 왜지? */
		m_pTransformCom->Set_State(CTransform::STATE_POSITION
			, XMVectorSetY(vPlayerPos, fJumpY + XMVectorGetY(vPlayerPos)));
	}

private:
	_bool	m_bIsJump = { false };
	_bool   m_bIsFalling = { false };
	_double m_UpTime = { 0.0 };
	_double m_DownTime = { 0.0 };
	_double m_JumpTimeAcc = { 0.0 };
	_float m_fPower;
#pragma endregion

protected:
	virtual void End_Animation(_uint& iSpriteIndex) override;
	virtual void Add_Animation() = 0;

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

