#pragma once

#include "Collider.h"

/* CColliderSphere2D 객체 하나는 하나의 충돌체를 의미한다. */

BEGIN(Engine)

class ENGINE_DLL CColliderSphere2D final : public CCollider
{
private:
	CColliderSphere2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderSphere2D(const CColliderSphere2D& rhs);
	virtual ~CColliderSphere2D() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;
	void Tick(_double TimeDelta) override;

public:
	virtual _bool IsCollision(CColliderAABB2D* pTarget) const override;
	virtual _bool IsCollision(CColliderOBB2D* pTarget) const override;
	virtual _bool IsCollision(CColliderSphere2D* pTarget) const override;

public:
	static CColliderSphere2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END