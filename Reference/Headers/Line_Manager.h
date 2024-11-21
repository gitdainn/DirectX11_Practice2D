#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CLineRider;

class CLine_Manager final : public CBase
{
	DECLARE_SINGLETON(CLine_Manager)

private:
	CLine_Manager();
	virtual ~CLine_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	_uint Tick(_double TimeDelta);
	_uint LateTick(_double TimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

public:
	void	Add_Vertex(const VertexPositionColor& tVertex);

	void	DeleteBack_Line();

public:
	const list<LINE_INFO>& Get_LineList() const { return m_LineList; }
	/** 탈 수 있는 가장 가까운 선을 얻습니다. */
	HRESULT Get_ClosestLineToRide(const _float2 vInObjectPosition, LINE_INFO** ppClosestLine);
	void	Scroll_Line(const _float fScrollX, const _float fScrollY);

private:
	// 두 점을 지나는 직선의 방정식 구하기 (return: 기울기)
	const _float Get_Slope(const _float3 fA, const _float3 fB);
	// 기울기와 직선을 지나는 한 점의 좌표를 알 때 절편 구하기 (return: y절편)
	const _float Get_InterceptY(const _float fSlope, const _float3 fB);
	// 기울기, 절편을 알 때 좌표 x를 지나는 직선 위의 점 y 구하기
	const _float EquationOfLine(const _float fSlope, const _float fInterceptY, const _float fX);
	const _float EquationOfLine(const _float3 fA, const _float3 fB, const _float fObjectX);

#ifdef _DEBUG
protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	VertexPositionColor*	m_pSourVertex = { nullptr };
	VertexPositionColor*	m_pDestVertex = { nullptr };

private:
	list<LINE_INFO> m_OriginalLineList;
	list<LINE_INFO>	m_LineList;

private:
	friend	class CLineRider;

public:
	virtual void Free() override;
};

END