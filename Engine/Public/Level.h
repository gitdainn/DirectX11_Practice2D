#pragma once

#include "Base.h"

/* 클라이언트에 정의한 레벨들의 부모클래스다. */
/* 레벨들의 공통적인 기능을 부여한다. */
/* 엔진에서 레벨을 보관하고자하는경우 타입(부모타입CLevel)을 제공해준다. */
/* 엔진에서 클라에 정의된 클래스의 멤버함수의 호출이 가능해지도록 한다.(함수 오버라이딩) */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END