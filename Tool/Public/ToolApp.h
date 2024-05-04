#pragma once

#include "Tool_Defines.h"
#include "Base.h"

//BEGIN(Engine)
//class CRenderer;
//class CGameInstance;
//END
//
//BEGIN(Tool)
//
//class CToolApp final : public CBase
//{
//private:
//	CToolApp();
//	virtual ~CToolApp() = default;
//
//public:
//	HRESULT Initialize();
//	void Tick(_double TimeDelta);
//	HRESULT Render();
//
//private:
//	ID3D11Device* m_pDevice = { nullptr };
//	ID3D11DeviceContext* m_pContext = { nullptr };
//
//private:
//	CGameInstance* m_pGameInstance = { nullptr };
//	CRenderer* m_pRenderer = { nullptr };
//
//#ifdef _DEBUG
//private:
//	_double						m_TimeAcc = { 0.0 };
//	_tchar						m_szFPS[MAX_PATH] = TEXT("");
//	_ulong						m_dwNumDraw = { 0 };
//#endif // _DEBUG
//
//private:
//	HRESULT SetUp_StartLevel(LEVEL eNextLevelID);
//	HRESULT Ready_Prototype_Component_For_Static();
//	HRESULT Ready_Prototype_GameObject_For_Static();
//	HRESULT Ready_Prototype_Sprite_For_Static();
//
//public:
//	static CToolApp* Create();
//	virtual void Free() override;
//};
//END

