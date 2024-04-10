#pragma once

#include "Base.h"

/* 모아서 관리 : 내가 나누고 싶은 기준(CLayer)에 따라 레벨[] 별로 */
/* 1. 원형 객체들을 모아서 관리하다. */
/* 1. 사본 객체들을 모아서 관리하다. */
/* 2. 사본객체들의 Tick, LateTick무한으로 호출한다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	void Clear(_uint iLevelIndex);

public:
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr); /* 원형을 복제하여 사본을 추가한다. */
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, const tSpriteInfo& SpriteInfo, void* pArg = nullptr); /* 원형을 복제하여 사본을 추가한다. */
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

private: /* 원형 객체들을 보관한다. */
	unordered_map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef unordered_map<const _tchar*, class CGameObject*>	PROTOTYPES;

private: /* 복제한 객체들을 그룹지어 레벨별로 보관한다. */
	unordered_map<const _tchar*, class CLayer*>*		m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*>	LAYERS;

private:
	_uint		m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	
public:
	virtual void Free() override;
};

END
