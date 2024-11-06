#pragma once

namespace Engine
{
	class ENGINE_DLL CUtil
	{
	public:
		static bool	Convert_CCW(_float3* _vPointA, _float3* _vPointB, _float3* _vPointC);
		static _int		Get_CCW(_float3 _vPointA, _float3 _vPointB, _float3 _vPointC);

		static XMMATRIX	Remove_Rotation(_fmatrix XMMATRIX);
		static XMMATRIX	Remove_Scale(_fmatrix XMMATRIX);
		static XMMATRIX	Remove_Position(_fmatrix XMMATRIX);

		// 3���� ���� 0.0 ~ 1.0 ������ ���� �̿��ؼ� ������ Ŀ�� ��ġ�� �˾� ���ϴ�.
		static _vector	Get_Bezier(_fvector _vStartPos, _fvector _vMiddlePos, _fvector _vEndPos, _double _T);
		// 4���� ���� 0.0 ~ 1.0 ������ ���� �̿��ؼ� ������ Ŀ�� ��ġ�� �˾� ���ϴ�.
		static _vector	Get_Bezier(_fvector _vStartPos, _fvector _vMiddlePos_1, _fvector _vMiddlePos_2, _gvector _vEndPos, _double _T);
		// 0.0 ~ 1.0 ���̸� �ε巴�� ��ȭ�ϴ� T�� ����
		static _double	GetT_Smooth(_double _T);
		// 0.0 ~ 1.0 ���̸� ó���� ������ ���� ������ ��ȭ�ϴ� T�� ����
		static _double	GetT_SlowToFast(_double _T);
		// 0.0 ~ 1.0 ���̸� ó���� ������ ���� ������ ��ȭ�ϴ� T�� ����
		static _double	GetT_FastToSlow(_double _T);

		/* CJW */
		/* ���� ��(x,y,z,w)�� Nan �� ������� true �� ��ȯ �մϴ�. */
		static bool	Get_VectorIsNan(_fvector vVector);
		/* �� ���� ��(x,y,z,w)�� ������ true �� ��ȯ �մϴ�. */
		static bool	Get_VectorIsEquel(_fvector vVector1, _fvector vVector2);
		/* ��ġ + ȸ�� �� �� �ִ� �ΰ��� ��Ʈ������ ���� �մϴ�. */
		static XMMATRIX	GetXMMATRIXLerp(_fmatrix Matrix1, _cmatrix Matrix2, _float t);


		static _double Get_MapingValue(_double Value, _double InputMin, _double InputMax, _double OutputMin, _double OutputMax);


		/*
		 ���� ����� �� ��Ʈ ��ķ� ��ȯ
		 ȭ�����
		 (��) <----------------- 0.f ---------------> (��)
		 (ViewPortSizeX * -0.5f) ~ (ViewPortSizeX * 0.5f)

		 (�ϴ�) <-------------- 0.f --------------> (���)
		 (ViewPortSizeY * -0.5f) ~ (ViewPortSizeY * 0.5f)
		*/
		static _vector	Convert_WorldToViewPort(_fvector _vWorldPos, _float2 _vViewPortSize);

		/* Lerp */
		static _float	Get_FloatLerp(_float fValue1, _float fValue2, _float _fT);

		/* ��Ŀ��� ȸ������ �����մϴ�. (����� �� �� Radian �Դϴ�.) (!!! ũ�⸦ ������ ��Ʈ������ ����ؾ� �մϴ�.) */
		static _float3 Get_ExtractRotationFromMatrix(XMMATRIX matrix);

	};

}