#include "Collision_Manager.h"
#include "ColliderAABB2D.h"
#include "ColliderOBB2D.h"
#include "ColliderSphere2D.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize(void* pArg)
{
	list<CCollider*> ListDefault;
	m_ColliderMap.emplace(L"Layer_Default", ListDefault);
	list<CCollider*> ListPlayer;
	m_ColliderMap.emplace(L"Layer_Player", ListPlayer);
	return S_OK;
}

_uint CCollision_Manager::Tick(_double TimeDelta)
{
	if (m_ColliderMap.empty())
		return _uint();

	return _uint();
}

_uint CCollision_Manager::LateTick(_double TimeDelta)
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

	// 아래 코드에서 같은 레이어끼리는 충돌 검사 안함
	if (1 >= m_ColliderMap.size())
		return _uint();

	for (auto iterSour = m_ColliderMap.begin(); iterSour != --m_ColliderMap.end(); ++iterSour)
	{
		auto iterDest = iterSour;
		++iterDest;
		for (iterDest ; iterDest != m_ColliderMap.end(); ++iterDest)
		{
			_tchar IgnoreLayer[MAX_PATH];
			lstrcpy(IgnoreLayer, iterSour->first);
			lstrcat(IgnoreLayer, iterDest->first);
			auto iter = m_IgnoreLayerMap.find(IgnoreLayer); // 문자열이 순서대로 저장되어 이써양함. 반대면 못 찾으니까

			if (iter != m_IgnoreLayerMap.end())
			{
				break;
			}

			Update_Collision(iterSour->second, iterDest->second);
		}
		
	}

	return _uint();
}

#ifdef _DEBUG
HRESULT CCollision_Manager::Render()
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
	}

	return S_OK;
}
#endif

void CCollision_Manager::Clear_Collider()
{
	auto iter = m_ColliderMap.begin();
	for (iter; iter != m_ColliderMap.end(); ++iter)
	{
		list<CCollider*> ColliderList = iter->second;
		for (CCollider* pCollider : ColliderList)
		{
			if (nullptr == pCollider)
				continue;

		}
		ColliderList.clear();
	}
	m_ColliderMap.clear();

	return;
}

HRESULT CCollision_Manager::Attach_Collider(const _tchar* pLayer, CCollider* pCollider)
{
	if (nullptr == pCollider || nullptr == pLayer)
	{
		MSG_BOX("CCollision_Manager - Attach_Collider - NULL");
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

void CCollision_Manager::Collision_Matrix()
{
}

HRESULT CCollision_Manager::Add_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB)
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

void CCollision_Manager::Update_Collision(list<CCollider*> pSourList, list<CCollider*> pDestList)
{
	for (CCollider* pSourCol : pSourList)
	{
		if (nullptr == pSourCol)
			continue;

		for (CCollider* pDestCol : pDestList)
		{
			if (nullptr == pDestCol)
				continue;

			_bool bHasBeenCollision = { false };
			COLLIDER_ID tID;
			tID.iSourID = pSourCol->Get_ID();
			tID.iDestID = pDestCol->Get_ID();

			m_CollisionMap.find(tID.ID);

			map<ULONGLONG, _bool>::iterator HasBeenCollisionIter = m_CollisionMap.find(tID.ID);
			if (m_CollisionMap.end() == HasBeenCollisionIter)
			{
				m_CollisionMap.emplace(tID.ID, bHasBeenCollision);
				HasBeenCollisionIter = m_CollisionMap.find(tID.ID);
			}

			// 현재 충돌 여부 검사
			bHasBeenCollision = HasBeenCollisionIter->second;
			if (pSourCol->IsCollision(pDestCol))
			{
				pSourCol->Set_Collision(true);
				pDestCol->Set_Collision(true);

				// 처음 부딪친 경우
				if (!bHasBeenCollision)
				{
					pSourCol->OnCollisionEnter(pDestCol);
					pDestCol->OnCollisionEnter(pSourCol);
					HasBeenCollisionIter->second = true;
				}
				// 부딪친 적이 있는 경우
				else
				{
					pSourCol->OnCollisionStay(pDestCol);
					pDestCol->OnCollisionStay(pSourCol);
				}
			}
			// 현재 부딪치지 않은 경우
			else
			{
				// 부딪친 적이 있는 경우
				if (bHasBeenCollision)
				{
					pSourCol->OnCollisionExit(pDestCol);
					pDestCol->OnCollisionExit(pSourCol);
					HasBeenCollisionIter->second = false;
				}
			}
		}
	}
}

bool CCollision_Manager::Is_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB) const
{
	return false;
}

void CCollision_Manager::Free()
{
	__super::Free();
}
