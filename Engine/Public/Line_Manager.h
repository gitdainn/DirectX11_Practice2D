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

	HRESULT Get_LandingPositionY(const _float2& vInObjectPosition, _float& vOutLandingY);

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

public:
	virtual void Free() override;
};

END