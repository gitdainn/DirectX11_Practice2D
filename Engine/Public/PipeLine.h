#pragma once

/* ��, ��������� �����ϰ� �ִ´�. */
/* �������ִ� ����. */

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);

	_float4x4 Get_Transform_Float4x4(TRANSFORMSTATE eState) const { 
		return m_TransformStateMatrix[eState]; }

	_matrix Get_Transform_Matrix(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformStateMatrix[eState]); }

	_matrix Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) {
		return XMLoadFloat4x4(&m_TransformStateMatrix_Inverse[eState]);
	}

	_float4x4 Get_Transform_Float4x4_Inverse(TRANSFORMSTATE eState) {
		return m_TransformStateMatrix_Inverse[eState];
	}

	_float4 Get_CamPosition() const {
		return m_vCamPosition;
	}

public:
	void Update();

private:
	_float4x4			m_TransformStateMatrix[D3DTS_END];
	_float4x4			m_TransformStateMatrix_Inverse[D3DTS_END];
	_float4				m_vCamPosition;

public:
	virtual void Free() override;	
};

END