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

	for (const auto& Pair : m_ColliderMap)
	{
		list<CCollider*> ColliderList = Pair.second;
		for (auto iter = ColliderList.begin() ; iter != ColliderList.end() ;)
		{
			if (nullptr == *iter)
				continue;

			if (OBJ_DEAD == (*iter)->Tick(TimeDelta))
			{
				iter = ColliderList.erase(iter);
			}
			else
				++iter;
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
			ID tID;
			tID.iSourID = find_if(m_LayerIDMap.begin(), m_LayerIDMap.end(), CTag_Finder(iterSour->first))->second; // 예외처리 .. 해야 하는데 ..
			tID.iDestID = find_if(m_LayerIDMap.begin(), m_LayerIDMap.end(), CTag_Finder(iterDest->first))->second;

			// 넣어주면 됨 ! 어차피 m_ColliderMap에 있따는 거부터가 ID는 부여됐따는 뜻.
			Sorting<_uint>()(tID.iSourID, tID.iDestID);
			auto LayerIter = m_CollisionLayerMap.find(tID.ID);
			if (m_CollisionLayerMap.end() == LayerIter)
			{
				m_CollisionLayerMap.emplace(tID.ID, false);
				continue;
			}
			if (true == LayerIter->second)
			{
				Update_Collision(iterSour->second, iterDest->second, iterSour->first, iterDest->first);
			}
		}
	}

	Track_ExitCollision();

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

HRESULT CCollision_Manager::Attach_Collider(const _tchar* pLayerTag, CCollider* pCollider)
{
	if (nullptr == pCollider)
	{
		MSG_BOX("CCollision_Manager - Attach_Collider - NULL");
		return S_OK;
	}

	// @note - 반복자, 함수 객체, 원시 자료형은 "값에 의한 전달"
	auto iter = find_if(m_ColliderMap.begin(), m_ColliderMap.end(), CTag_Finder(pLayerTag));
	if (m_ColliderMap.end() == iter)
	{
		list<CCollider*> ColliderList;
		ColliderList.emplace_back(pCollider);
		m_ColliderMap.emplace(pLayerTag, ColliderList);

		auto IDIter = find_if(m_LayerIDMap.begin(), m_LayerIDMap.end(), CTag_Finder(pLayerTag));
		if(m_LayerIDMap.end() == IDIter)
			m_LayerIDMap.emplace(pLayerTag, ++m_iLayerID);
	}
	else
	{
		iter->second.emplace_back(pCollider);
	}
	return S_OK;
}

void CCollision_Manager::Set_CollisionLayerMatrix(const _tchar* pLayerSour, const _tchar* pLayerDest, const _bool bIsCollision)
{
	if (nullptr == pLayerSour || nullptr == pLayerDest)
		return;

	ID tID;
	auto iter = find_if(m_LayerIDMap.begin(), m_LayerIDMap.end(), CTag_Finder(pLayerSour));
	if (m_LayerIDMap.end() == iter)
	{
		m_LayerIDMap.emplace(pLayerSour, ++m_iLayerID);
		tID.iDestID = m_iLayerID;
	}
	else
	{
		tID.iDestID = iter->second;
	}

	iter = find_if(m_LayerIDMap.begin(), m_LayerIDMap.end(), CTag_Finder(pLayerDest));
	if (m_LayerIDMap.end() == iter)
	{
		m_LayerIDMap.emplace(pLayerDest, ++m_iLayerID);
		tID.iSourID = m_iLayerID;
	}
	else
	{
		tID.iSourID = iter->second;
	}

	Sorting<_uint>()(tID.iSourID, tID.iDestID);
	unordered_map<ULONGLONG, _bool>::iterator LayerIter = m_CollisionLayerMap.find(tID.ID);
	if (m_CollisionLayerMap.end() == LayerIter)
	{
		m_CollisionLayerMap.emplace(tID.ID, bIsCollision);
	}
	else
	{
		LayerIter->second = bIsCollision;
	}

	return;
}

void CCollision_Manager::Update_Collision(list<CCollider*> pSourColList, list<CCollider*> pDestColList, const _tchar* pSourLayer, const _tchar* pDestLayer)
{
	set<ULONGLONG>		DualColliderIDSet;

	for (CCollider* pSourCol : pSourColList)
	{
		if (nullptr == pSourCol)
			continue;

		for (CCollider* pDestCol : pDestColList)
		{
			if (nullptr == pDestCol)
				continue;

			_bool bHasBeenCollision = { false };
			ID tID;
			tID.iSourID = pSourCol->Get_ID();
			tID.iDestID = pDestCol->Get_ID();

			map<ULONGLONG, _bool>::iterator HasBeenCollisionIter = m_CollisionColliderMap.find(tID.ID);
			if (m_CollisionColliderMap.end() == HasBeenCollisionIter)
			{
				m_CollisionColliderMap.emplace(tID.ID, bHasBeenCollision);
				HasBeenCollisionIter = m_CollisionColliderMap.find(tID.ID);
			}
			m_CurrentTickDualIDSet.emplace(tID.ID);

			// 현재 충돌 여부 검사
			bHasBeenCollision = HasBeenCollisionIter->second;
			if (pSourCol->IsCollision(pDestCol))
			{
				pSourCol->Set_Collision(true);
				pDestCol->Set_Collision(true);

				// 처음 부딪친 경우
				if (!bHasBeenCollision)
				{
					pSourCol->OnCollisionEnter(pDestCol, pDestLayer);
					pDestCol->OnCollisionEnter(pSourCol, pSourLayer);
					HasBeenCollisionIter->second = true;
				}
				// 부딪친 적이 있는 경우
				else
				{
					pSourCol->OnCollisionStay(pDestCol, pDestLayer);
					pDestCol->OnCollisionStay(pSourCol, pSourLayer);
				}

				COLLIDER_INFO tColliderInfo;
				tColliderInfo.pSourCollider = pSourCol;
				tColliderInfo.pDestCollider = pDestCol;
				tColliderInfo.pSourLayer = pSourLayer;
				tColliderInfo.pDestLayer = pDestLayer;
				m_CollisionTrueColliderInfoMap.emplace(tID.ID, tColliderInfo);
			}
			// 현재 부딪치지 않은 경우
			else
			{
				// 부딪친 적이 있는 경우
				if (bHasBeenCollision)
				{
					pSourCol->OnCollisionExit(pDestCol, pDestLayer);
					pDestCol->OnCollisionExit(pSourCol, pSourLayer);
					HasBeenCollisionIter->second = false;

					auto Iter = m_CollisionTrueColliderInfoMap.erase(tID.ID);
				}
			}
		}
	}

}

void CCollision_Manager::Track_ExitCollision()
{
	for (auto iter = m_CollisionColliderMap.begin(); iter != m_CollisionColliderMap.end(); ++iter)
	{
		const ULONGLONG CollisionID = iter->first;

		if (false == iter->second)
			continue;

		const auto DualIDSetIter = m_CurrentTickDualIDSet.find(CollisionID);
		if (m_CurrentTickDualIDSet.end() == DualIDSetIter)
		{
			auto Infoiter = m_CollisionTrueColliderInfoMap.find(CollisionID);
			COLLIDER_INFO tColliderInfo = Infoiter->second;
			if(!tColliderInfo.pSourCollider->Get_IsDead())
				tColliderInfo.pSourCollider->OnCollisionExit(tColliderInfo.pDestCollider, tColliderInfo.pDestLayer);

			if(!tColliderInfo.pDestCollider->Get_IsDead())
				tColliderInfo.pDestCollider->OnCollisionExit(tColliderInfo.pSourCollider, tColliderInfo.pSourLayer);
			
			auto Iter = m_CollisionTrueColliderInfoMap.erase(CollisionID);
			iter->second = false;
		}
	}

	m_CurrentTickDualIDSet.clear();

}

void CCollision_Manager::Free()
{
	__super::Free();
}
