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
	HRESULT	Awaken();

protected:
	virtual void Add_Animation();
	virtual void Play_Animation(_uint& iSpriteIndex, _double TimeDelta);

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

private:
	_uint m_iMaxLevel;
	_uint m_iLevel;
	_float m_fSpeed;

	_double m_CoolTime;
	_double m_CycleTime;
	_double m_LifeTime;

public:
	/* Prototype */
	static CSkill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;

};

