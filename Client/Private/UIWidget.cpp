#include "UIWidget.h"

HRESULT CUIWidget::Add_Widget(const _tchar* pWidgetName, CGameObject* pObject)
{
	if (nullptr == pWidgetName || nullptr == pObject)
		return E_FAIL;

	auto CompareLambda = [pWidgetName](const pair<const _tchar*, list<CGameObject*>>& Pair)
		{
			return !lstrcmp(Pair.first, pWidgetName);
		};

	auto iter = find_if(m_WidgetUIMap.begin(), m_WidgetUIMap.end(), CompareLambda);

	if (iter == m_WidgetUIMap.end())
	{
		list<CGameObject*> ObjectList;
		ObjectList.emplace_back(pObject);
		m_WidgetUIMap.emplace(pWidgetName, pObject);
	}
	else
	{
		iter->second.emplace_back(pObject);
	}

	return S_OK;
}

list<CGameObject*>* CUIWidget::Get_ObjectList(const _tchar* pWidgetName)
{
	if (nullptr == pWidgetName)
		return nullptr;

	auto CompareLambda = [pWidgetName](const pair<const _tchar*, list<CGameObject*>>& Pair)
		{
			return !lstrcmp(Pair.first, pWidgetName);
		};

	auto iter = find_if(m_WidgetUIMap.begin(), m_WidgetUIMap.end(), CompareLambda);
	if (iter == m_WidgetUIMap.end())
		return nullptr;
	
	return &iter->second;
}

CGameObject* CUIWidget::Get_GameObjectByName(const _tchar* pWidgetName, const _tchar* pObjName)
{
	if (nullptr == pWidgetName || nullptr == pObjName)
		return nullptr;

	list<CGameObject*>* pObjectList = Get_ObjectList(pWidgetName);
	if (nullptr == pObjectList)
		return nullptr;

	CGameObject* pTargetObject = { nullptr };
	for (CGameObject* pObj : *pObjectList)
	{
		if (nullptr == pObj)
			continue;

		if (!lstrcmp(pObjName, pObj->Get_NameTag()))
		{
			pTargetObject = pObj;
			break;
		}
	}

	return pTargetObject;
}
