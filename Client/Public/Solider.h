#pragma once
#include "Enemy.h"

class CSolider final : public CEnemy
{
public:
	explicit CSolider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CSolider() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void Add_Animation() override;

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

protected:
	virtual void Idle(_double TimeDelta) override;
	virtual void Walk(_double TimeDelta) override;
	virtual void Attack(_double TimeDelta);
	virtual void Chase(_double TimeDelta) override;
	virtual void Damaged(_double TimeDelta) override;

public:
	/* Prototype */
	static CSolider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

