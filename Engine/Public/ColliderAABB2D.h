#pragma once

#include "Collider.h"

/* CColliderAABB2D 객체 하나는 하나의 충돌체를 의미한다. */

BEGIN(Engine)

class ENGINE_DLL CColliderAABB2D final : public CCollider
{
private:
	CColliderAABB2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderAABB2D(const CColliderAABB2D& rhs);
	virtual ~CColliderAABB2D() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	void Tick(_double TimeDelta) override;

protected:
	virtual _bool Intersects(CColliderAABB2D* pTarget) const override;
	virtual _bool Intersects(CColliderOBB2D* pTarget) override;
	virtual _bool Intersects(CColliderSphere2D* pTarget) override;

public:
	static CColliderAABB2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END