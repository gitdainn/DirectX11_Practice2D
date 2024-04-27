#pragma once

#include "Component.h"

/* 객체의 월드 변환을 위한 월드변환행렬을 가지고 있는다.*/
/* 객체를 렌더링할 때에 셰이더에 월드행렬을 전달해주는 과정을 수행한다. */
/* 월드 행렬을 통해 수행할 수 있는 다양한 월드 변환의 기능(움직임제어)을 가진다. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_double		SpeedPerSec;
		_double		RotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public: /* Getter */
	_vector Get_State(STATE eState) {		
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
		// return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);		
	}

	_float3 Get_Scaled() {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldMatrixFloat() {
		return m_WorldMatrix;
	}

public: /* Setter */
	void Set_WorldMatrix(_matrix matWorld) {
		_matrix matWorldW = matWorld;
		matWorldW.r[(_uint)STATE::STATE_POSITION] = XMVectorSetW(matWorldW.r[3], 1.f);
		XMStoreFloat4x4(&m_WorldMatrix, matWorldW);
	}

	void Set_State(STATE eState, _fvector vState) {		
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}

	void Set_TransformDesc(const TRANSFORM_DESC& TransformDesc) {
		m_TransformDesc = TransformDesc; }

	void Set_Scaled(const _float3& vScale);

	HRESULT Set_ShaderResource(class CShader* pShader, const char* pContantName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Go_Straight(_double TimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Backward(_double TimeDelta);

	/* 정적인 회전. : 항등상태기준 vAxis축으로 Radian만큼 회전해 있는다. */
	void Rotation(_fvector vAxis, _double Radian);

	/* 동적인 회전. : 현재상태기준 vAxis축으로 m_TransformDesc.fRotationSpeedPerSec만큼 회전한다 */
	void Turn(_fvector vAxis, _double TimeDelta);

	void LookAt(_fvector vTargetPos);

	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDistance = 0.2f);

private:
	/* 단순 보관용 데이터. */
	/* 월드 스페이스로의 변환. */
	/* 월드 공간에서 객체의 right, up, look, position을 행순서로 저장한\ 데이터. */
	/* 직교행렬. */
	_float4x4			m_WorldMatrix;
	// _matrix : 행렬의 연산을 위한 데이터 타입. 
	TRANSFORM_DESC		m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END