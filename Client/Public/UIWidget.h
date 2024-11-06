#pragma once
#include "Widget.h"

#include "UI.h"
class CUIWidget final : public CWidget
{
public:
	HRESULT	Add_Widget(const _tchar* pWidgetName, CGameObject* pObject);

public:
	list<CGameObject*>* Get_ObjectList(const _tchar* pWidgetName);
	CGameObject* Get_GameObjectByName(const _tchar* pWidgetName, const _tchar* pObjName);

private:
	unordered_map<const _tchar*, list<CGameObject*>>	m_WidgetUIMap;
};

