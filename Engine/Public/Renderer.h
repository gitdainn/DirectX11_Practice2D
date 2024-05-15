#pragma once

#include "Component.h"
#include <queue>
/* 0.화면에 그려져야할 객체들을 그려야하는 순서에따라 모아서 보관한다. */
/* 1.보관하고 있는 객체들의 드로우콜을 수행한다. */

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_DebugRenderGroup(class CComponent* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	/** @note - 함수객체
	* 1. 함수객체는 operator()로 선언하며, 클래스/구조체 이름으로 호출함.
	* 2. 조건자에 사용되며 bool로 반환 
	* */
	struct Sort
	{
		bool operator()(CGameObject* pA, CGameObject* pB);
	};

private:		
	list<CGameObject*>			m_RenderGroups[RENDER_END];
	/** @note - 우선순위큐
	* 1. 정렬 기준을 정한다면 함수객체를 직접 만들고 priority_queue<객체, 함수객체 또는 조건자>
	* */
	// @qurious - 우선순위큐 정렬: O(log n), vector 또는 <algorithm>의 sort는 O(n)?
	// @qurious - 한 번만 정렬 시 sort가 효율, 삽입/삭제 빈번 시 우선순위큐가 효율?
	// priority_queue<class CGameObject*, Sort>	m_RenderGroupsQueue[RENDER_END];

private:
	list<CComponent*>				m_DebugGroup;

private:
	class CLight_Manager*				m_pLight_Manager = { nullptr };


private:
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };
	class CShader*						m_pShader = { nullptr };

private:
	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	

private: /* 그리는 그룹들에 따라 셋팅이 바뀌어야할 필요가 생길 수 있기때문에 그룹별로 함수를 만들어 처리. */
	HRESULT Draw_Priority();
	HRESULT Draw_NonBlend();
	HRESULT Draw_Blend();
	HRESULT Draw_UI();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END