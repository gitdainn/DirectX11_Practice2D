#pragma once
#include "SpriteObject.h"
#include "Player_Manager.h"
#include <functional>


class CItem abstract : public CSpriteObject
{
public:
	explicit CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CItem(const CItem& rhs);
	~CItem() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget) override;

protected:
	virtual void End_Animation(_uint& iSpriteIndex) override;
	virtual void Add_Animation() = 0;
	void	Landing_Ground();

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

