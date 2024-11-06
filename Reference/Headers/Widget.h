#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CWidget : public CComponent
{
public:
	struct WIDGET_ARGUMENT
	{
		CGameObject* pOwner = { nullptr };
		const _tchar* pFilePath = { nullptr };
	};

protected:
	CWidget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWidget(const CWidget& rhs);
	virtual ~CWidget() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	_uint Tick_BindToOwner(_double TimeDelta);

public:
	HRESULT	Add_Widget(CGameObject* pObject);

public:
	list<CGameObject*>* Get_WidgetList() { return &m_WidgetList; }
	CGameObject* Get_WidgetByName(const _tchar* pObjName);

protected:
	void	Notify_OwnerDead();

protected:
	list<CGameObject*>	m_WidgetList;

public:
	static CWidget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END