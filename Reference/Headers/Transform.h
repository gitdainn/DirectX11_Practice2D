#pragma once

#include "Component.h"

/* ��ü�� ���� ��ȯ�� ���� ���庯ȯ����� ������ �ִ´�.*/
/* ��ü�� �������� ���� ���̴��� ��������� �������ִ� ������ �����Ѵ�. */
/* ���� ����� ���� ������ �� �ִ� �پ��� ���� ��ȯ�� ���(����������)�� ������. */

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

	/* ������ ȸ��. : �׵���±��� vAxis������ Radian��ŭ ȸ���� �ִ´�. */
	void Rotation(_fvector vAxis, _double Radian);

	/* ������ ȸ��. : ������±��� vAxis������ m_TransformDesc.fRotationSpeedPerSec��ŭ ȸ���Ѵ� */
	void Turn(_fvector vAxis, _double TimeDelta);

	void LookAt(_fvector vTargetPos);

	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDistance = 0.2f);

private:
	/* �ܼ� ������ ������. */
	/* ���� �����̽����� ��ȯ. */
	/* ���� �������� ��ü�� right, up, look, position�� ������� ������\ ������. */
	/* �������. */
	_float4x4			m_WorldMatrix;
	// _matrix : ����� ������ ���� ������ Ÿ��. 
	TRANSFORM_DESC		m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END