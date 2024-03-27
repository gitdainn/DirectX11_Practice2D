#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

	_bool isIn_World(_fvector vPosition, _float fRange);
	_bool isIn_Local(_fvector vPosition, _float fRange);
	void  Transform_ToLocalSpace(_fmatrix WorldMatrix);

private:
	_float3				m_vPoints_Original[8];
	_float3				m_vPoints[8];

	_float4				m_Planes_World[6];
	_float4				m_Planes_Local[6];

private:
	HRESULT Make_Plane(const _float3* pPoints, _float4* pPlanes);

public:
	virtual void Free() override;
};

END
