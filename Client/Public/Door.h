#pragma once

#include "Client_Defines.h"
#include "AnimObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CDoor final : public CAnimObject
{
public:
	// 스컬 문 종류: 골드룸, 아이템/정수룸, 스컬룸, 용사룸, 암시장룸, 각성/보스룸
	enum DOOR_TYPE { GOLD, ITEM_ESSENCE, SKUL, HERO, BLACKMARKET, AWAKENING_BOSS, DOOR_END };

private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체의 초기화작업 */
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override; /* 사본객체의 초기화작업 */
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 던진다. */
	virtual void Add_Animation() override;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override;

public:
	/* Prototype */
	/* 원형 객체를 생성한다. */
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본 객체를 생성한다. */
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

END