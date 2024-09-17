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
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	SkillTick(_double TimeDelta);
	void	SkillLateTick(_double TimeDelta);
	void	SkillRender();

public:
	virtual void	Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection = SPRITE_DIRECTION::DIRECTION_END);
	void	Execute_Skill(_uint iSkillIndex);

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget) override;

public:
	void	Set_IsEquipped(const bool bIsEquipped)
	{
		m_bIsEquipped = bIsEquipped;
	}
	void	Set_IsInAir(const bool bInAir)
	{
		m_bIsInAir = bInAir;
	}

protected:
	virtual void End_Animation(_uint& iSpriteIndex) override;
	void	Mapping_SkulData(const _tchar* pObjectID);
	CSkill*	Get_Skill(const _tchar* pObjectID);
	virtual void Add_Animation() = 0;
	void Awaken();

private:
	void	Mapping_Type(const SKUL_TYPE& tType);
	void	Landing_Ground();
	
protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

protected:
	CSkill* m_pSkill[iSkillNum] = { nullptr };
	CCollider* m_pDefaultAtkColliderCom = { nullptr };
	//unordered_map<const CONTROL_KEY, _uint>	m_ControlMap;
	_bool	m_bIsEquipped;

private:
	list<CSkill*> m_SkillAvailableList;

protected:
	SKUL_RANK	m_eSkulRank;
	SKUL_TYPE	m_eSkulType;

	_uint	m_iLevel = { 1 };

	_int m_iHp = { 100 };
	_int m_iMagicAttack = { 10 };
	_int m_iPhysicalAttack = { 10 };
	_int m_iDefense = { 10 };
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
	/* Prototype */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

