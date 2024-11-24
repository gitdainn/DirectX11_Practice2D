#pragma once
#include "Skill.h"

class CSkillDive final : public CSkill
{
public:
	explicit CSkillDive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CSkillDive() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enter(CAnimObject* pOwner = nullptr) override;
	virtual HRESULT	Awaken() override;

protected:
	virtual void Add_Animation() override;
	virtual void End_Animation(_uint& iSpriteIndex) override;

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 던진다. */

public:
	/* Prototype */
	static CSkillDive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const override;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;

};

