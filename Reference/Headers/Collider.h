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
		_float3		vScale = { 1.f, 1.f, 1.f };
		_float3		vRotation = { 0.f, 0.f, 0.f };
		_float3		vPosition = { 0.f, 0.f, 0.f };
		_float3		vOffset = { 0.f, 0.f, 0.f };
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
	virtual _bool IsCollision(CColliderAABB2D* pTarget) const = 0;
	virtual _bool IsCollision(CColliderOBB2D* pTarget) const = 0;
	virtual _bool IsCollision(CColliderSphere2D* pTarget) const = 0;

public:
	virtual void Set_Owner(CGameObject* pOwner) override;

public:
	virtual const HRESULT Get_ComponentInfo(COMPONENT_INFO& tComponentInfo) const override
	{
		tComponentInfo.pPrototypeTag = m_pPrototypeTag;
		tComponentInfo.fSize.x = m_tColliderDesc.vScale.x;
		tComponentInfo.fSize.y = m_tColliderDesc.vScale.y;
		tComponentInfo.fOffset.x = m_tColliderDesc.vOffset.x;
		tComponentInfo.fOffset.y = m_tColliderDesc.vOffset.y;
		tComponentInfo.fPosition.x = m_tColliderDesc.vPosition.x;
		tComponentInfo.fPosition.y = m_tColliderDesc.vPosition.y;

		return S_OK;
	}

	const COLLIDER_DESC& Get_ColliderDesc() const
	{
		return m_tColliderDesc;
	}

	void Set_ColliderDesc(const COLLIDER_DESC& tColliderDesc)
	{
		memcpy(&m_tColliderDesc, &tColliderDesc, sizeof(COLLIDER_DESC));
	}

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
	_float4x4				m_WorldMatrix;
	COLLIDER_DESC			m_tColliderDesc;
	_bool					m_bIsCollision = { false };

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;
};

END