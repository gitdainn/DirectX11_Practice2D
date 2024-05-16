#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex);
	list<class CGameObject*>* Get_ObjectList()
	{
		return &m_GameObjectList;
	}

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

private:
	list<class CGameObject*>				m_GameObjectList;	

public:
	static CLayer* Create();
	virtual void Free() override;
};

END