#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CDInput_Manager : public CBase
{
	DECLARE_SINGLETON(CDInput_Manager)

public:
	enum class MOUSEKEYSTATE { DIMK_LB, DIMK_RB, DIMK_MB, DIMK_END };
	enum class MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_W, DIMM_END };

public:
	enum class INPUTSTATE { IS_NONE, IS_DOWN, IS_STAY, IS_UP, IS_END };

private:
	typedef struct InputInfo
	{
		INPUTSTATE eState;		//	입력 상태
		bool bIsPreviousPush;	// 이전에 눌렸는지 확인 하는 값
	}INPUTINFO;

private:
	CDInput_Manager();
	virtual ~CDInput_Manager() = default;

public:
	HRESULT			Ready_DInput(HINSTANCE hInst, HWND hWnd);
	void			Update_DInput(void);

public:
	_bool		Get_KeyStay(_ubyte ubyKeyID)
	{
		return (m_tKeyInputState[ubyKeyID].eState == INPUTSTATE::IS_STAY);
	}
	_bool		Get_MouseStay(MOUSEKEYSTATE eMouseID)
	{
		return (m_tMouseInputState[(_uint)eMouseID].eState == INPUTSTATE::IS_STAY);
	}
	_long		Get_MouseMove(MOUSEMOVESTATE eMouseMoveID)
	{
		return *(((_long*)&m_MouseState) + (_uint)eMouseMoveID);
	}

public:
	_bool		Get_KeyDown(_ubyte ubyKeyID);
	_bool		Get_KeyUp(_ubyte ubyKeyID);

	_bool		Get_MouseDown(MOUSEKEYSTATE eMouseID);
	_bool		Get_MouseUp(MOUSEKEYSTATE eMouseID);

private:
	LPDIRECTINPUT8				m_pInputSDK = { nullptr };

	LPDIRECTINPUTDEVICE8		m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8		m_pMouse = { nullptr };

private:
	_byte					m_byKeyState[256];		// Engine용
	DIMOUSESTATE			m_MouseState;			// Engine용

private:
	INPUTINFO				m_tKeyInputState[256];	// Client 제공용
	INPUTINFO				m_tMouseInputState[3];	// Client 제공용

public:
	virtual void Free() override;
};

END