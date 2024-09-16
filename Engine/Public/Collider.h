#pragma once

#include "Component.h"
#include "Engine_Defines.h"
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
	virtual void Tick(_double TimeDelta);

public:
	_vector	Get_IntersectVectorX(CCollider* pTargetCollider)
	{
		COLLIDER_DESC tTargetDesc = pTargetCollider->Get_ColliderDesc();

		// 도착 위치 - 현재 위치 = 도착 위치를 바라보는 방향벡터
		//_vector vPushDirection = XMLoadFloat3(&tTargetDesc.vPosition) - XMLoadFloat3(&m_tColliderDesc.vPosition) * -1;
		_vector vPushDirection = XMLoadFloat3(&m_tColliderDesc.vPosition) - XMLoadFloat3(&tTargetDesc.vPosition);
		_float vPushDistance = (m_tColliderDesc.vScale.x * 0.5f + tTargetDesc.vScale.x * 0.5f) - XMVectorGetX(XMVector3Length(vPushDirection));

		// 방향 * 길이
		return XMVector3Normalize(vPushDirection) * vPushDistance;
	}

public:
	_bool IsCollision(CCollider* pTargetCollider);
	void OnCollisionEnter(CCollider* pTargetCollider);
	void OnCollisionStay(CCollider* pTargetCollider);
	void OnCollisionExit(CCollider* pTargetCollider);

protected:
	virtual _bool Intersects(CColliderAABB2D* pTarget) const = 0;
	virtual _bool Intersects(CColliderOBB2D* pTarget) = 0;
	virtual _bool Intersects(CColliderSphere2D* pTarget) = 0;

public:
	virtual void Set_Owner(CGameObject* pOwner) override;

	void		Set_IsBlock(const _bool& bIsBlock)
	{
		m_bIsBlock = bIsBlock;
	}

	void		Set_Collision(const _bool& bIsCollision)
	{
		m_bIsCollision = bIsCollision;
	}

	void Set_ColliderDesc(const COLLIDER_DESC& tColliderDesc)
	{
		memcpy(&m_tColliderDesc, &tColliderDesc, sizeof(COLLIDER_DESC));
	}

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

	const _bool& Get_IsBlock() const
	{
		return m_bIsBlock;
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
	_bool					m_bIsBlock = { false };

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;
};

END