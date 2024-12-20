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

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

private:
	list<class CGameObject*>				m_GameObjects;	

public:
	static CLayer* Create();
	virtual void Free() override;
};

END