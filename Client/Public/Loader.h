#pragma once

/* 스레드를 통하여 특정 레벨의 준비를 한다. */

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool Get_Finished() const {
		return m_isFinished;
	}

	CRITICAL_SECTION Get_CriticalSection() {
		return m_CriticalSection;
	}

	LEVEL Get_NextLevelID() const {
		return m_eNextLevelID;
	}

	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading_ForLogoLevel();
	HRESULT Loading_Sprite_Logo();
	HRESULT Loaiding_GameObject_Logo();

	HRESULT Loading_ForGamePlayLevel();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	LEVEL				m_eNextLevelID = { LEVEL_END };
	HANDLE				m_hThread = { 0 };
	CRITICAL_SECTION	m_CriticalSection;
	_bool				m_isFinished = { false };
	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free();
};

END