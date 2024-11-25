#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();	

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	CGameInstance*				m_pGameInstance = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };

#ifdef _DEBUG
private:
	_double						m_TimeAcc = { 0.0 };
	_tchar						m_szFPS[MAX_PATH] = TEXT("");
	_ulong						m_dwNumDraw = { 0 };
#endif // _DEBUG

private:
	HRESULT SetUp_StartLevel(LEVEL eNextLevelID);

	/** 레이어별 충돌 행렬입니다. */
	HRESULT Ready_CollisionLayerMatrix();
	HRESULT Ready_DefaultData_Excel();
	HRESULT Ready_Prototype_Sprite_For_Static();
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_GameObject_For_Static();
	HRESULT Ready_PrototypeName();

private:
	HRESULT Add_SpriteObject(const _tchar* pFileName, const SPRITE_INFO& tSprite);

public:
	static CMainApp* Create();
	virtual void Free() override;
};
END

