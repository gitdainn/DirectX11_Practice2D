#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CLine_Manager final : public CBase
{
	DECLARE_SINGLETON(CLine_Manager)

private:
	CLine_Manager();
	virtual ~CLine_Manager() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

public:
	void	Add_Vertex(const VertexPositionColor& tVertex)
	{
		if (nullptr == m_pDestVertex)
		{
			m_pDestVertex = new VertexPositionColor();
			memcpy(m_pDestVertex, &tVertex, sizeof(VertexPositionColor));
			return;
		}

		else if (nullptr == m_pSourVertex)
		{
			m_pSourVertex = new VertexPositionColor();
			memcpy(m_pSourVertex, &tVertex, sizeof(VertexPositionColor));
		}

		LINE_INFO Line;
		if ((*m_pDestVertex).position.x < (*m_pSourVertex).position.x)
		{
			memcpy(&(Line.tLeftVertex), m_pDestVertex, sizeof(VertexPositionColor));
			memcpy(&(Line.tRightVertex), m_pSourVertex, sizeof(VertexPositionColor));
		}
		else
		{
			memcpy(&(Line.tLeftVertex), m_pSourVertex, sizeof(VertexPositionColor));
			memcpy(&(Line.tRightVertex), m_pDestVertex, sizeof(VertexPositionColor));
		}

		m_LineList.emplace_back(Line); // 내 기억상.... 컨테이너에 넣을 때 복사본이라.... &원본이여도 괜찮을듯 하지만 일단 지켜보기!

		Safe_Delete(m_pDestVertex);
		Safe_Delete(m_pSourVertex);
	}

	void	DeleteBack_Line()
	{
		if (m_LineList.empty())
			return;

		list<LINE_INFO>::iterator iterLast = m_LineList.end();
		m_LineList.erase(--iterLast);
	}

public:
	const list<LINE_INFO>& Get_LineList() const
	{
		return m_LineList;
	}
	
	/** @notice - IsCurrentLineOccupied 함수를 먼저 호출한 뒤 사용해주세요. */
	HRESULT Get_CurrentLineEndPoint(const _float2 vObjectPosition, pair<_float3, _float3>& EndPoints)
	{			
		EndPoints = make_pair(_float3(m_tClosestLandingLine.tLeftVertex.position), _float3(m_tClosestLandingLine.tRightVertex.position));
		return S_OK;
	}

	/** 발밑에 탈 수 있는 선이 있는지 검사합니다. */
	bool HasPassableLine(const _float2 vInObjectPosition, _float& fOutLandingY);

	/** 현재 선을 타고 있는 중인지 검사합니다. */
	bool IsCurrentLineOccupied(const _float2 vObjectPosition, _float& fOutLandingY);

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
	list<LINE_INFO>	m_LineList;
	LINE_INFO		m_tClosestLandingLine;

	//priority_queue<_float, vector<_float>, greater<_float>> m_PossibleLandingQueue;

public:
	virtual void Free() override;
};

END