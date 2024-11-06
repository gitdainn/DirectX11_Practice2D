#pragma once
#include "HealthBarUI.h"
#include "Player.h"

class CEnemyHealthBar final : public CHealthBarUI
{
protected:
	CEnemyHealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyHealthBar(const CEnemyHealthBar& rhs);
	virtual ~CEnemyHealthBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Owner(CPlayer* pPlayer) { m_pOwner = pPlayer; }

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	void	Follow_Owner() const;

private:
	CPlayer* m_pOwner = { nullptr };
	_float2	m_fOffset = { 0.f,  0.f };

public:
	static  CEnemyHealthBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

