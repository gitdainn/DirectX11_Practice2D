#pragma once

#include "Component.h"

/* CCollider 객체 하나는 하나의 충돌체를 의미한다. */

BEGIN(Engine)

class CGameObject;
class CColliderAABB2D;
class CColliderOBB2D;
class CColliderSphere2D;

class CCollider abstract : public CComponent
{
public:
	typedef struct tagColliderDesc
	{
		CGameObject* pOwner = { nullptr };
		_float3		vScale;
		_float3		vRotation;
		_float3		vPosition;				
	}COLLIDER_DESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_fmatrix TransformMatrix = XMMatrixIdentity());

public:
	const COLLIDER_DESC& Get_ColliderDesc() const
	{
		return m_tColliderDesc;
	}

	virtual _bool IsCollision(CColliderAABB2D* pTarget) const = 0;
	virtual _bool IsCollision(CColliderOBB2D* pTarget) const = 0;
	virtual _bool IsCollision(CColliderSphere2D* pTarget) const = 0;

protected:
	_matrix Remove_Rotation(_fmatrix TranformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

#ifdef _DEBUG
protected:
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*				m_pEffect = { nullptr };
	ID3D11InputLayout*			m_pInputLayout = { nullptr };
#endif // _DEBUG

protected:
	CGameObject*			m_pOwner;
	COLLIDER_DESC			m_tColliderDesc;
	_bool					m_bIsCollision = { false };

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;
};

END