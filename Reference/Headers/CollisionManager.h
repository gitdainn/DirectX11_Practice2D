#pragma once
#include "Base.h"
#include "Engine_Defines.h"
#include "Collider.h"

BEGIN(Engine)

class CCollisionManager : public CBase
{
	DECLARE_SINGLETON(CCollisionManager)

private:
	CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

public:
	HRESULT	Attach_Collider(COLLIDER_LAYER eLayer, CCollider* pCollider);
	void	Collision_Matrix();

private:
	void	Check_Collision(_double TimeDelta);

private:
	list<CCollider*>	m_ColliderList[(_uint)COLLIDER_LAYER::LAYER_END];

public:
	virtual void Free() override;
};

END