#pragma once
#include "Item.h"
#include "Player_Manager.h"


class CSkulItem final : public CItem
{
public:
	explicit CSkulItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkulItem(const CSkulItem& rhs);
	~CSkulItem() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

protected:
	virtual void End_Animation(_uint& iSpriteIndex) override;
	virtual void Add_Animation();

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

public:
	/* Prototype */
	static CSkulItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;

};

