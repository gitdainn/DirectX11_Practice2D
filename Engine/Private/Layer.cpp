#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_GameObjectList.end())
		return nullptr;

	return (*iter)->Get_Component(pComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjectList.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	if (m_GameObjectList.empty())
		return;

	auto iter = m_GameObjectList.begin();
	for (iter ; iter != m_GameObjectList.end() ; )
	{
		if (nullptr == *iter)
		{
			++iter;
			continue;
		}

		if (OBJ_DEAD == (*iter)->Tick(TimeDelta))
		{
			// @note - erase는 현재 반복자 삭제 후 다음 반복자를 반환함
			Safe_Release(*iter);
			iter = m_GameObjectList.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

void CLayer::LateTick(_double TimeDelta)
{
	for (CGameObject* pGameObject : m_GameObjectList)
	{
		if (nullptr != pGameObject)
			pGameObject->LateTick(TimeDelta);
	}
}

CLayer * CLayer::Create()
{
	CLayer*			pLayer = new CLayer();

	return pLayer;	
}

void CLayer::Free()
{
	for (CGameObject* pGameObject : m_GameObjectList)
		Safe_Release(pGameObject);

	m_GameObjectList.clear();
}

