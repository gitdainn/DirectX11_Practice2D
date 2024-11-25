#pragma once
#include "AnimObject.h"

class CSkill abstract : public CAnimObject
{
public:
	explicit CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CSkill() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Late_Initialize();
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enter(CAnimObject* pOwner = nullptr);
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
	virtual HRESULT SetUp_ShaderResources() = 0;
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
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
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

