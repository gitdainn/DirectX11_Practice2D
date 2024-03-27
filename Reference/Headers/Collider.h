#pragma once

#include "Component.h"

/* CCollider 객체 하나는 하나의 충돌체를 의미한다. */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3		vScale;
		_float3		vRotation;
		_float3		vPosition;				
	}COLLIDERDESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	HRESULT Initialize_Prototype(TYPE eType);
	HRESULT Initialize(void* pArg) override;
	void Tick(_fmatrix TransformMatrix);
	_bool Collision(CCollider* pTarget);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	BoundingBox*			m_pAABB_Original = { nullptr };
	BoundingOrientedBox*	m_pOBB_Original = { nullptr };
	BoundingSphere*			m_pSphere_Original = { nullptr };

	BoundingBox*			m_pAABB = { nullptr };
	BoundingOrientedBox*	m_pOBB = { nullptr };
	BoundingSphere*			m_pSphere = { nullptr };

	TYPE					m_eType = { TYPE_END };
	_bool					m_isCollision = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*				m_pEffect = { nullptr };
	ID3D11InputLayout*			m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	_matrix Remove_Rotation(_fmatrix TranformMatrix);



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END