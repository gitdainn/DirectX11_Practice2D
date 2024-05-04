#include "DInput_Manager.h"

IMPLEMENT_SINGLETON(CDInput_Manager)

CDInput_Manager::CDInput_Manager()
{
	ZeroMemory(m_byKeyState, sizeof m_byKeyState);
	ZeroMemory(m_tKeyInputState, sizeof m_tKeyInputState);

	ZeroMemory(&m_MouseState, sizeof m_MouseState);
	ZeroMemory(m_tMouseInputState, sizeof m_tMouseInputState);
}

HRESULT CDInput_Manager::Ready_DInput(HINSTANCE hInst, HWND hWnd)
{
	// 부모 컴객체 생성
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	// 내 컴퓨터에 연결되어 있는 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체애 대한 정보를 컴객체에게 전달
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 생성된 키보드 객체의 동작 범위를 설정
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 생성된 키보드 컴객체를 활성화
	m_pKeyBoard->Acquire();


	// 내 컴퓨터에 연결되어 있는 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체애 대한 정보를 컴객체에게 전달
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 생성된 마우스 객체의 동작 범위를 설정
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 생성된 마우스 컴객체를 활성화
	m_pMouse->Acquire();

	return S_OK;
}

void CDInput_Manager::Update_DInput(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	// 장치에서 받아온 정보를 바탕으로 한번의 Keyinput의 결과를 Client의 여러 객체에게 공통적인 결과로 반환
	// +) 객체가 함수를 호출할 때마다 검사해서 결과를 알려주는 것이 아닌 엔진에서 일괄적으로 검사하고 true, false만 반환 -> 동시 입력 가능해짐

	// KeyInput
	for (_uint i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] & 0x80)		// 키를 누른 경우
		{
			if (true == m_tKeyInputState[i].bIsPreviousPush)
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_STAY;	// 이전에 눌렀었고 현재도 누른 경우 -> KeyStay
			}
			else
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_DOWN;	// 이전에 안 눌렀었고 현재는 누른 경우 -> KeyDown
			}

			m_tKeyInputState[i].bIsPreviousPush = true;		// 다음 Tick을 위해 이전(현재 Tick)에 해당 키를 눌렀다고 저장
		}
		else	// 키를 누르지 않은 경우
		{
			if (true == m_tKeyInputState[i].bIsPreviousPush)
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_UP;		// 이전에 눌렀었고 현재는 누르지 않은 경우 -> KeyUp
			}
			else
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_NONE;	// 이전에 안 눌렀었고 현재도 누르지 않은 경우 -> KeyNone
			}

			m_tKeyInputState[i].bIsPreviousPush = false;	// 다음 Tick을 위해 이전(현재 Tick)에 해당 키를 눌르지 않았다고 저장
		}
	}

	// MouseInput
	for (_uint i = 0; i < (_uint)MOUSEKEYSTATE::DIMK_END; ++i)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)	// 마우스 클릭한 경우
		{
			if (true == m_tMouseInputState[i].bIsPreviousPush)
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_STAY;		// 이전에 클릭했었고 현재도 클릭한 경우 -> MouseStay
			}
			else
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_DOWN;		// 이전에 클릭 안했었고 현재는 클릭한 경우 -> MouseDown
			}

			m_tMouseInputState[i].bIsPreviousPush = true;	// 다음 Tick을 위해 이전(현재 Tick)에 클릭했었다고 저장
		}
		else	// 클릭 안 한 경우
		{
			if (true == m_tMouseInputState[i].bIsPreviousPush)
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_UP;		// 이전에 클릭 안 했었고 현재는 클릭한 경우 -> MouseUp
			}
			else
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_NONE;		// 이전에 클릭 안 했었고 현재도 클릭 안 한 경우 -> MouseNone
			}

			m_tMouseInputState[i].bIsPreviousPush = false;	// 다음 Tick을 위해 이전(현재 Tick)에 클릭 안 했었다고 저장
		}
	}
}

_bool CDInput_Manager::Get_KeyDown(_ubyte ubyKeyID)
{
	return (m_tKeyInputState[ubyKeyID].eState == INPUTSTATE::IS_DOWN);
}

_bool CDInput_Manager::Get_KeyUp(_ubyte ubyKeyID)
{
	return (m_tKeyInputState[ubyKeyID].eState == INPUTSTATE::IS_UP);
}

_bool CDInput_Manager::Get_MouseDown(MOUSEKEYSTATE eMouseID)
{
	return (m_tMouseInputState[(_uint)eMouseID].eState == INPUTSTATE::IS_DOWN);
}

_bool CDInput_Manager::Get_MouseUp(MOUSEKEYSTATE eMouseID)
{
	return (m_tMouseInputState[(_uint)eMouseID].eState == INPUTSTATE::IS_UP);
}

void CDInput_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
