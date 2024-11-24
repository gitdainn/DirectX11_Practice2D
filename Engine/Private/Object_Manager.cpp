#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(pComponentTag, iIndex);	
}

list<CGameObject*>* CObject_Manager::Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ObjectList();
}

CGameObject* CObject_Manager::Get_GameObjectByName(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pObjName)
{
	if (nullptr == pObjName)
		return nullptr;

	list<CGameObject*>* pObjectList = Get_ObjectList(iLevelIndex, pLayerTag);
	if (nullptr == pObjectList)
		return nullptr;

	for (CGameObject* pObject : *pObjectList)
	{
		if (!lstrcmp(pObject->Get_NameTag(), pObjName))
		{
			return pObject;
		}
	}
	return nullptr;
}

CGameObject* CObject_Manager::Get_LastObject(_uint iLevelIndex, const _tchar* pLayerTag)
{
	list<CGameObject*>* pObjectList = Get_ObjectList(iLevelIndex, pLayerTag);
	if (nullptr == pObjectList)
		return nullptr;

	if (pObjectList->empty())
		return nullptr;

	return pObjectList->back();
}

CGameObject* CObject_Manager::Get_LastObject(_uint iLevelIndex, const _tchar* pLayerTag, const _uint iNum)
{
	list<CGameObject*>* pObjectList = Get_ObjectList(iLevelIndex, pLayerTag);
	if (nullptr == pObjectList)
		return nullptr;

	if (0 >= iNum || pObjectList->size() < iNum)
		return nullptr;

	auto iter = pObjectList->end();
	for (_uint i = 0; i < iNum; ++i)
	{
		--iter;
	}

	return *iter;
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayersMap)
		return E_FAIL;

	m_pLayersMap = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayersMap[iLevelIndex])
		Safe_Release(Pair.second);
	m_pLayersMap[iLevelIndex].clear();
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg)
{
	/* 원형을 찾는다. */
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 사본을 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayersMap[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}

	return S_OK;
}

/* 원형객체를 찾아 복제하여 레이어에 추가한다. */
HRESULT CObject_Manager::Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, const SPRITE_INFO& tSpriteInfo, void* pArg)
{
	/* 원형을 찾는다. */
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 사본을 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(tSpriteInfo, pArg);
	if (nullptr == pGameObject)
		return E_FAIL;	

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;	

		m_pLayersMap[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;
	}	

	return S_OK;
}

/* 원형을 찾아 복제하여 리턴한다. */
CGameObject* CObject_Manager::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	/* 원형을 찾는다. */
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	/* 사본을 생성한다. */
	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;
	pGameObject->Late_Initialize();

	return pGameObject;	
}

CGameObject* CObject_Manager::Clone_GameObject(const _tchar* pPrototypeTag, const SPRITE_INFO& tSpriteInfo, void* pArg)
{
	/* 원형을 찾는다. */
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	/* 사본을 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(tSpriteInfo, pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

HRESULT CObject_Manager::Late_Initialize()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayersMap[i])
		{
			if (nullptr == Pair.second)
			{
				MSG_BOX("CObject_Manager - Tick - Null");
				continue;
			}
			Pair.second->Late_Initialize();
		}
	}
	return S_OK;
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayersMap[i])
		{
			if (nullptr == Pair.second)
			{
				MSG_BOX("CObject_Manager - Tick - Null");
				continue;
			}
			Pair.second->Tick(TimeDelta);			
		}
	}	
}

void CObject_Manager::LateTick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayersMap[i])
		{
			Pair.second->LateTick(TimeDelta);
		}
	}
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	CompareLambda = [pLayerTag](const pair<const _tchar*, CLayer*>& Pair)
		{
			return !lstrcmp(pLayerTag, Pair.first);
		};

	// @note - find_if문은 람다에 소괄호를 생략하면 자동으로 pair가 소괄호에 들어감
	unordered_map<const _tchar*, class CLayer*>::iterator iter 
		= find_if(m_pLayersMap[iLevelIndex].begin(), m_pLayersMap[iLevelIndex].end(), CompareLambda);

	if (iter == m_pLayersMap[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		if (nullptr == m_pLayersMap)
			break;

		for (auto& Pair : m_pLayersMap[i])
			Safe_Release(Pair.second);
		m_pLayersMap[i].clear();
	}
	Safe_Delete_Array(m_pLayersMap);	

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}
