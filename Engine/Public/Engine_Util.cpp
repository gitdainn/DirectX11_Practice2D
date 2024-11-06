#pragma once
#include "Engine_Defines.h"
#include "Pipeline.h"

namespace Engine
{
	_bool CUtil::Convert_CCW(_float3* _vPointA, _float3* _vPointB, _float3* _vPointC)
	{
		/* �ð� ������ ������ ���� �˻��Ѵ�. */
		_float3		vPoints[3];
		if (-1 == Get_CCW(*_vPointA, *_vPointB, *_vPointC))
		{
			vPoints[0] = *_vPointA;
			vPoints[1] = *_vPointB;
			vPoints[2] = *_vPointC;
		}
		else if (-1 == Get_CCW(*_vPointA, *_vPointC, *_vPointB))
		{
			vPoints[0] = *_vPointA;
			vPoints[1] = *_vPointC;
			vPoints[2] = *_vPointB;
		}
		else if (-1 == Get_CCW(*_vPointB, *_vPointC, *_vPointA))
		{
			vPoints[0] = *_vPointB;
			vPoints[1] = *_vPointC;
			vPoints[2] = *_vPointA;
		}
		else if (-1 == Get_CCW(*_vPointB, *_vPointA, *_vPointC))
		{
			vPoints[0] = *_vPointB;
			vPoints[1] = *_vPointA;
			vPoints[2] = *_vPointC;
		}
		else if (-1 == Get_CCW(*_vPointC, *_vPointB, *_vPointA))
		{
			vPoints[0] = *_vPointC;
			vPoints[1] = *_vPointB;
			vPoints[2] = *_vPointA;

		}
		else if (-1 == Get_CCW(*_vPointC, *_vPointA, *_vPointB))
		{
			vPoints[0] = *_vPointC;
			vPoints[1] = *_vPointA;
			vPoints[2] = *_vPointB;
		}
		else
		{
			return false;
		}

		*_vPointA = vPoints[0];
		*_vPointB = vPoints[1];
		*_vPointC = vPoints[2];

		return true;
	}

	_int CUtil::Get_CCW(_float3 _vPointA, _float3 _vPointB, _float3 _vPointC)
	{
		_float temp1 = (_vPointB.z - _vPointA.z) * (_vPointC.x - _vPointA.x) + _vPointA.z * (_vPointB.x - _vPointA.x);
		_float temp2 = (_vPointB.x - _vPointA.x) * _vPointC.z;

		if (temp1 < temp2)
		{
			return 1; /* �ݽð� */
		}
		else if (temp1 > temp2)
		{
			return -1; /* �ð� */
		}
		else
		{
			return 0; /* ������ */
		}
	}

	_matrix CUtil::Remove_Rotation(_fmatrix _Matrix)
	{
		_matrix matResult = _Matrix;

		matResult.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * XMVectorGetX(XMVector3Length(matResult.r[0]));
		matResult.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * XMVectorGetX(XMVector3Length(matResult.r[1]));
		matResult.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * XMVectorGetX(XMVector3Length(matResult.r[2]));

		return matResult;
	}

	_matrix Engine::CUtil::Remove_Scale(_fmatrix _Matrix)
	{
		_matrix		matResult = _Matrix;

		matResult.r[0] = XMVector3Normalize(matResult.r[0]);
		matResult.r[1] = XMVector3Normalize(matResult.r[1]);
		matResult.r[2] = XMVector3Normalize(matResult.r[2]);

		return matResult;
	}

	_matrix Engine::CUtil::Remove_Position(_fmatrix _Matrix)
	{
		_matrix		matResult = _Matrix;

		matResult.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		return matResult;
	}

	_vector Engine::CUtil::Get_Bezier(_fvector _vStartPos, _fvector _vMiddlePos, _fvector _vEndPos, _double _T)
	{
		// Clamp value to 0 to 1
		_T = (_T > 1.0) ? 1.0 : ((_T < 0.0) ? 0.0 : _T);

		// 1st Interpolation
		_vector vA = XMVectorLerp(_vStartPos, _vMiddlePos, (_float)_T);
		_vector vB = XMVectorLerp(_vMiddlePos, _vEndPos, (_float)_T);

		// 2st Interpolation
		_vector vC = XMVectorLerp(vA, vB, (_float)_T);

		return XMVectorSetW(vC, 1.f);
	}

	_vector Engine::CUtil::Get_Bezier(_fvector _vStartPos, _fvector _vMiddlePos_1, _fvector _vMiddlePos_2, _gvector _vEndPos, _double _T)
	{
		// Clamp value to 0 to 1
		_T = (_T > 1.0) ? 1.0 : ((_T < 0.0) ? 0.0 : _T);

		// 1st Interpolation
		_vector vA = XMVectorLerp(_vStartPos, _vMiddlePos_1, (_float)_T);
		_vector vB = XMVectorLerp(_vMiddlePos_1, _vMiddlePos_2, (_float)_T);
		_vector vC = XMVectorLerp(_vMiddlePos_2, _vEndPos, (_float)_T);

		// 2st Interpolation
		_vector vD = XMVectorLerp(vA, vB, (_float)_T);
		_vector vE = XMVectorLerp(vB, vC, (_float)_T);

		// 3st Interpolation
		_vector vF = XMVectorLerp(vD, vE, (_float)_T);

		return XMVectorSetW(vF, 1.f);
	}

	_double Engine::CUtil::GetT_Smooth(_double _T)
	{
		_T = (_T > 1.0) ? 1.0 : ((_T < 0.0) ? 0.0 : _T);
		_T = _T * _T * (3.f - 2.f * _T);

		return _T;
	}
	_double CUtil::GetT_SlowToFast(_double _T)
	{
		_T = (_T > 1.0) ? 1.0 : ((_T < 0.0) ? 0.0 : _T);
		_T = pow(_T, 3.0);

		return _T;
	}
	_double CUtil::GetT_FastToSlow(_double _T)
	{
		_T = (_T > 1.0) ? 1.0 : ((_T < 0.0) ? 0.0 : _T);
		_T = cbrt(_T);

		return _T;
	}

	_bool CUtil::Get_VectorIsNan(_fvector vVector)
	{
		_vector vNanTest = XMVectorIsNaN(vVector);

		if (0 == XMVectorGetX(vNanTest) &&
			0 == XMVectorGetY(vNanTest) &&
			0 == XMVectorGetZ(vNanTest) &&
			0 == XMVectorGetW(vNanTest))
		{
			return false;
		}
		return true;
	}
	_bool CUtil::Get_VectorIsEquel(_fvector vVector1, _fvector vVector2)
	{
		_vector vEquelTest = XMVectorEqual(vVector1, vVector2);

		if (0 == XMVectorGetX(vEquelTest) &&
			0 == XMVectorGetY(vEquelTest) &&
			0 == XMVectorGetZ(vEquelTest) &&
			0 == XMVectorGetW(vEquelTest))
		{
			return false;
		}
		return true;
	}

	_matrix CUtil::Get_MatrixLerp(_fmatrix Matrix1, _cmatrix Matrix2, _float t)
	{
		// ��Ŀ��� ��ġ �� ȸ�� ��� ����
		XMVECTOR translation1 = Matrix1.r[3];
		XMVECTOR translation2 = Matrix2.r[3];
		XMMATRIX rotationMatrix1 = XMMatrixIdentity();
		XMMATRIX rotationMatrix2 = XMMatrixIdentity();
		for (int i = 0; i < 3; ++i)
		{
			rotationMatrix1.r[i] = Matrix1.r[i];
			rotationMatrix2.r[i] = Matrix2.r[i];
		}

		// ��ġ�� ����
		XMVECTOR interpolatedTranslation = XMVectorLerp(translation1, translation2, t);

		// ȸ�� ����� ���ʹϾ����� ��ȯ
		XMVECTOR quaternion1 = XMQuaternionRotationMatrix(rotationMatrix1);
		XMVECTOR quaternion2 = XMQuaternionRotationMatrix(rotationMatrix2);

		// ���ʹϾ��� ����
		XMVECTOR interpolatedQuaternion = XMQuaternionSlerp(quaternion1, quaternion2, t);

		// ������ ��ġ�� ȸ�� ����� �����Ͽ� ��ȯ ��� ����
		XMMATRIX interpolatedMatrix = XMMatrixIdentity();
		interpolatedMatrix.r[3] = interpolatedTranslation;
		interpolatedMatrix *= XMMatrixRotationQuaternion(interpolatedQuaternion);

		return interpolatedMatrix;
	}

	_double CUtil::Get_MapingValue(_double Value, _double InputMin, _double InputMax, _double OutputMin, _double OutputMax)
	{
		// �Է� ���������� ���� ���� ������ ����մϴ�.
		_double inputRange = InputMax - InputMin;
		_double outputRange = OutputMax - OutputMin;
		_double scaledValue = (Value - InputMin) / inputRange;

		// ������ ��� ������ �����Ͽ� ��ȯ�մϴ�.
		return OutputMin + (scaledValue * outputRange);
	}

	_float CUtil::Get_FloatLerp(_float fValue1, _float fValue2, _float _fT)
	{
		_fT = (_fT > 1.f) ? 1.f : ((_fT < 0.f) ? 0.f : _fT);
		return fValue1 * (1.0f - _fT) + fValue2 * _fT;
	}

	_float3 CUtil::Get_ExtractRotationFromMatrix(_matrix matrix)
	{
		/* ��ķκ��� ȸ���� ��Ÿ���� Quaternion �� ���մϴ�. */
		_vector vQuaternion = DirectX::XMQuaternionRotationMatrix(matrix);
		_float4 vRotationQuat;
		DirectX::XMStoreFloat4(&vRotationQuat, vQuaternion);

		/* �� ���� ȸ�� ���� ���մϴ�. */
		_float3 vRotation;
		vRotation.x = atan2f(2 * (vRotationQuat.y * vRotationQuat.z + vRotationQuat.w * vRotationQuat.x),
			vRotationQuat.w * vRotationQuat.w - vRotationQuat.x * vRotationQuat.x - vRotationQuat.y * vRotationQuat.y + vRotationQuat.z * vRotationQuat.z);
		vRotation.y = asinf(-2 * (vRotationQuat.x * vRotationQuat.z - vRotationQuat.w * vRotationQuat.y));
		vRotation.z = atan2f(2 * (vRotationQuat.x * vRotationQuat.y + vRotationQuat.w * vRotationQuat.z),
			vRotationQuat.w * vRotationQuat.w + vRotationQuat.x * vRotationQuat.x - vRotationQuat.y * vRotationQuat.y - vRotationQuat.z * vRotationQuat.z);

		return vRotation;
	}
}