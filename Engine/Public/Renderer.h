#pragma once

#include "Component.h"

/* 0.화면에 그려져야할 객체들을 그려야하는 순서에따라 모아서 보관한다. */
/* 1.보관하고 있는 객체들의 드로우콜을 수행한다. */

BEGIN(Engine)

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
	list<class CGameObject*>			m_RenderGroups[RENDER_END];

private:
	list<class CComponent*>				m_DebugGroup;

private:
	class CTarget_Manager*				m_pTarget_Manager = { nullptr };
	class CLight_Manager*				m_pLight_Manager = { nullptr };


private:
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };
	class CShader*						m_pShader = { nullptr };

private:
	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	

private: /* 그리는 그룹들에 따라 셋팅이 바뀌어야할 필요가 생길 수 있기때문에 그룹별로 함수를 만들어 처리. */
	HRESULT Draw_Priority();
	HRESULT Draw_NonBlend();
	HRESULT Draw_NonLight();
	HRESULT Draw_Blend();
	HRESULT Draw_UI();

private:
	HRESULT Draw_LightAcc();
	HRESULT Draw_DeferredBlend();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG




public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END