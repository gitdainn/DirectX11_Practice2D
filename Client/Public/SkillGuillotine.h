#pragma once
#include "Skill.h"

class CSkillGuillotine final : public CSkill
{
public:
	explicit CSkillGuillotine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CSkillGuillotine() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Enter(CSpriteObject* pOwner = nullptr) override;
	virtual HRESULT	Awaken() override;

protected:
	virtual void Add_Animation() override;
	virtual void End_Animation(_uint& iSpriteIndex) override;

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

public:
	/* Prototype */
	static CSkillGuillotine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const override;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;

};

