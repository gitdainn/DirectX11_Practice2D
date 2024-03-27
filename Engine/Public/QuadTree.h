#pragma once

#include "Base.h"

/* 가장 먼저만드는 CQuadTree라는 객체는 지형 전체의 정보를 가진 쿼드트리의 노드다. */
BEGIN(Engine)

class CQuadTree final : public CBase
{

public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum NEIGHBOR {
		NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(class CFrustum* pFrustum, _float3* pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces);
	HRESULT SetUp_Neighbors();
private:
	_uint					m_iCorners[CORNER_END] = { 0 };
	_uint					m_iCenter = { 0 };
	_float					m_fRadius = { 0.0 };
	class CQuadTree*		m_pChilds[CHILD_END] = { nullptr };
	class CQuadTree*		m_pNeighbors[NEIGHBOR_END] = { nullptr };

private:

	_bool LevelofDetail(_float3* pVerticesPos);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END