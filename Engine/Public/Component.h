#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);	
	virtual HRESULT Render() { return S_OK; }

public:
	const _tchar* Get_PrototypeTag() const
	{
		return m_pPrototypeTag;
	}

	void Set_PrototypeTag(const _tchar* pPrototypeTag)
	{
		m_pPrototypeTag = pPrototypeTag;
	};

	virtual void Set_Owner(CGameObject* pOwner)
	{
		if (nullptr == pOwner)
		{
			MSG_BOX("CComponent - Set_Owner - NULL");
			return;
		}
		m_pOwner = pOwner;
	}

	virtual const HRESULT Get_ComponentInfo(COMPONENT_INFO& tComponentInfo) const
	{
		return E_FAIL;
	}

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	CGameObject*				m_pOwner = { nullptr };
	const _tchar*				m_pPrototypeTag = { nullptr };
	_bool						m_isCloned = { false };

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END