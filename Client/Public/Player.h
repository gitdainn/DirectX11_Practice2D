#pragma once
#include "SpriteObject.h"

const int iSkillNum = { 2 };

class CSkill;

class CPlayer : public CSpriteObject
{
public:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CPlayer() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	SkillTick(_double TimeDelta);
	void	SkillLateTick(_double TimeDelta);
	void	SkillRender();

public:
	void	Swap_Skill() { m_eCurrentState = STATE_TYPE::SWAP; Change_AnimType((_uint)STATE_TYPE::SWAP); }
	virtual void	Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection = SPRITE_DIRECTION::DIRECTION_END);
	void	Execute_Skill(_uint iSkillIndex);

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

public:
	void	Set_IsEquipped(const bool bIsEquipped)
	{
		m_bIsEquipped = bIsEquipped;
	}
	void	Set_IsInAir(const bool bInAir)
	{
		m_bIsInAir = bInAir;
	}

public:
	/** @notice - Add_PrototypeName()으로 스킬 이름 및 프로토타입 이름을 미리 넣어주세요. */
	CSkill* Get_Skill(const _tchar* pObjectID);

protected:
	virtual void	End_Animation(_uint& iSpriteIndex) override;
	virtual	HRESULT JumpableLineRider(_double TimeDelta) override;

	virtual void Add_Animation() = 0;
	void	Mapping_SkulData(const _tchar* pObjectID);
	void Awaken();


private:
	void	Mapping_Type(const SKUL_TYPE& tType);

protected:
	/** @param - 공격 상태일 때 공격 콜라이더를 넣을 인덱스를 넣어주세요. (m_iUVTextureIndex 기준으로 비교) */
	void	Add_DefaultAtkCollider(const _uint iTextureIndex);

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

protected:
	CSkill* m_pSkill[iSkillNum] = { nullptr };
	CCollider* m_pDefaultAtkColliderCom = { nullptr };

	_double	m_SwapTimeAcc = { 0.0 };
	_bool	m_bIsEquipped;
	_bool	m_bIsSwap;

private:
	list<CSkill*> m_SkillAvailableList;

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

