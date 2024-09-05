#include "CollisionManager.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

HRESULT CCollisionManager::Initialize(void* pArg)
{
	return S_OK;
}

_uint CCollisionManager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < (_uint)COLLIDER_LAYER::LAYER_END; ++i)
	{
		for (CCollider* pCollider : m_ColliderList[i])
		{
			if (nullptr == pCollider)
				continue;

			pCollider->Tick(TimeDelta);
		}
	}

	return _uint();
}

_uint CCollisionManager::LateTick(_double TimeDelta)
{
#ifdef _DEBUG
	Render();
#endif
	
	for (_uint i = 0; i < (_uint)COLLIDER_LAYER::LAYER_END; ++i)
	{
		m_ColliderList[i].clear();
	}

	return _uint();
}

#ifdef _DEBUG
HRESULT CCollisionManager::Render()
{
	for (_uint i = 0; i < (_uint)COLLIDER_LAYER::LAYER_END; ++i)
	{
		for (CCollider* pCollider : m_ColliderList[i])
		{
			if (nullptr == pCollider)
				continue;

			pCollider->Render();
		}

	}
	return S_OK;
}
#endif

HRESULT CCollisionManager::Attach_Collider(COLLIDER_LAYER eLayer, CCollider* pCollider)
{
	if (nullptr == pCollider)
	{
		MSG_BOX("CCollisionManager - Attach_Collider - NULL");
		return S_OK;
	}

	m_ColliderList[(_uint)eLayer].emplace_back(pCollider);
	return S_OK;
}

void CCollisionManager::Collision_Matrix()
{
}
void CCollisionManager::Check_Collision(_double TimeDelta)
{
}

void CCollisionManager::Free()
{
}
