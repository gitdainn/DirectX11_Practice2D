#pragma once
#include "SpriteObject.h"

class CSkill abstract : public CSpriteObject
{
public:
	explicit CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CSkill() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enter(CSpriteObject* pOwner = nullptr);
	virtual HRESULT	Awaken();

public:
	const _bool& Get_IsSkillAvailable()
	{
		return m_bIsSkillAvailable;
	}

	const _bool& Get_IsDelayFinished()
	{
		return m_bIsDelayFinished;
	}

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */
	void	Mapping_SkillData(const _tchar* pObjectID);

protected:
	void	ResetTime()
	{
		m_AnimAcc = 0.0;
		m_CoolDownAcc = 0.0;
		m_CycleTimeAcc = 0.0;
		m_LifeTimeAcc = 0.0;
		m_DelayTimeAcc = 0.0;
	}

	HRESULT	Landing_Ground(const _vector& vPosition);

protected:
	_uint m_iMaxLevel;
	_uint m_iLevel;
	_float m_fSpeed;

	_double m_CoolDown;
	_double m_CycleTime;
	_double m_LifeTime;
	_double m_DelayTime;

	_double m_CoolDownAcc;
	_double m_CycleTimeAcc;
	_double m_LifeTimeAcc;
	_double m_DelayTimeAcc;

	_bool	m_bIsSkillAvailable = { true };
	_bool	m_bIsDelayFinished = { false };

	int		m_iDamage[3];

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

