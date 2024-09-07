#include "CollisionManager.h"
#include "ColliderAABB2D.h"
#include "ColliderOBB2D.h"
#include "ColliderSphere2D.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

HRESULT CCollisionManager::Initialize(void* pArg)
{
	list<CCollider*> ListDefault;
	m_ColliderMap.emplace(L"Layer_Default", ListDefault);
	list<CCollider*> ListPlayer;
	m_ColliderMap.emplace(L"Layer_Player", ListPlayer);
	return S_OK;
}

_uint CCollisionManager::Tick(_double TimeDelta)
{
	if (m_ColliderMap.empty())
		return _uint();

	auto iter = m_ColliderMap.begin();
	for (iter; iter != m_ColliderMap.end(); ++iter)
	{
		list<CCollider*> ColliderList = iter->second;
		for (CCollider* pCollider : ColliderList)
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
	if (m_ColliderMap.empty())
		return _uint();

	_tchar IgnoreLayer[MAX_PATH];
	for (auto iterSour = m_ColliderMap.begin(); iterSour != --m_ColliderMap.end(); ++iterSour)
	{
		bool bHasCheck = { false };
		auto iterDest = iterSour;
		for (++iterDest ; iterDest != m_ColliderMap.end(); ++iterDest)
		{
			if (!bHasCheck)
			{
				lstrcpy(IgnoreLayer, iterSour->first);
				lstrcat(IgnoreLayer, iterDest->first);
				auto iter = m_IgnoreLayerMap.find(IgnoreLayer); // 흠 근데.. 빨리 찾는 것 외에는 second가 필ㅇ 없음

				if (iter != m_IgnoreLayerMap.end())
				{
					break; // 해당for문 벗어나고 다음 레이어 검사하라는 뜻)
				}

				bHasCheck = true;
			}

			Update_Collision(iterSour->second, iterDest->second);
		}
	}

	return _uint();
}

#ifdef _DEBUG
HRESULT CCollisionManager::Render()
{
	auto iter = m_ColliderMap.begin();
	for (iter; iter != m_ColliderMap.end(); ++iter)
	{
		list<CCollider*> ColliderList = iter->second;
		for (CCollider* pCollider : ColliderList)
		{
			if (nullptr == pCollider)
				continue;

			pCollider->Render();
		}
		ColliderList.clear();
	}
	m_ColliderMap.clear();

	return S_OK;
}
#endif

HRESULT CCollisionManager::Attach_Collider(const _tchar* pLayer, CCollider* pCollider)
{
	if (nullptr == pCollider || nullptr == pLayer)
	{
		MSG_BOX("CCollisionManager - Attach_Collider - NULL");
		return S_OK;
	}

	map<const _tchar*, list<CCollider*>>::iterator iter = m_ColliderMap.find(pLayer);
	if (m_ColliderMap.end() == iter)
	{
		list<CCollider*> ColliderList;
		ColliderList.emplace_back(pCollider);
		m_ColliderMap.emplace(pLayer, ColliderList);
	}
	else
	{
		iter->second.emplace_back(pCollider);
	}
	return S_OK;
}

void CCollisionManager::Collision_Matrix()
{
}

HRESULT CCollisionManager::Add_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB)
{
	if (nullptr == pLayerA || nullptr == pLayerB)
		return E_FAIL;

	// 먼저 레이어 우선순위 비교한 다음에!
	_tchar* pIgnoreLayer = new _tchar[lstrlen(pLayerA) + lstrlen(pLayerB) + 1];
	lstrcpy(pIgnoreLayer, pLayerA);
	lstrcat(pIgnoreLayer, pLayerB);

	m_IgnoreLayerMap.insert(pIgnoreLayer);

	return S_OK;
}

void CCollisionManager::Update_Collision(list<CCollider*> pSourList, list<CCollider*> pDestList) const
{
	for (CCollider* pSourCol : pSourList)
	{
		if (nullptr == pSourCol)
			continue;

		for (CCollider* pDestCol : pDestList)
		{
			if (nullptr == pDestCol)
				continue;

			
			if (pSourCol->IsCollision(pDestCol))
			{
				pDestCol->Set_Collision(true);

			}
		}
	}
}

bool CCollisionManager::Is_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB) const
{
	return false;
}

void CCollisionManager::Free()
{
}
