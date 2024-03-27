#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)

private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pContext);
	HRESULT Set_ShaderResourceView(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	ID3D11RenderTargetView*			m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView*			m_pDepthStencilView = { nullptr };

private: /* 렌더타겟을 생성하여 보관의 역할을 한다. */
	unordered_map<const _tchar*, class CRenderTarget*>				m_RenderTargets;
	typedef unordered_map<const _tchar*, class CRenderTarget*>		RENDERTARGETS;

private:
	/* 생성된 렌더타겟들 중, 동시에 바인딩 되어야할 렌더타겟들을 미리 묶어놓는다. */
	unordered_map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef unordered_map<const _tchar*, list<class CRenderTarget*>>	MRTS;

private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END