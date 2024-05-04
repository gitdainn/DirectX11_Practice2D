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
	// �θ� �İ�ü ����
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �İ�ü���� ����
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ������ Ű���� ��ü�� ���� ������ ����
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ Ű���� �İ�ü�� Ȱ��ȭ
	m_pKeyBoard->Acquire();


	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �İ�ü���� ����
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ������ ���콺 ��ü�� ���� ������ ����
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ ���콺 �İ�ü�� Ȱ��ȭ
	m_pMouse->Acquire();

	return S_OK;
}

void CDInput_Manager::Update_DInput(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	// ��ġ���� �޾ƿ� ������ �������� �ѹ��� Keyinput�� ����� Client�� ���� ��ü���� �������� ����� ��ȯ
	// +) ��ü�� �Լ��� ȣ���� ������ �˻��ؼ� ����� �˷��ִ� ���� �ƴ� �������� �ϰ������� �˻��ϰ� true, false�� ��ȯ -> ���� �Է� ��������

	// KeyInput
	for (_uint i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] & 0x80)		// Ű�� ���� ���
		{
			if (true == m_tKeyInputState[i].bIsPreviousPush)
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_STAY;	// ������ �������� ���絵 ���� ��� -> KeyStay
			}
			else
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_DOWN;	// ������ �� �������� ����� ���� ��� -> KeyDown
			}

			m_tKeyInputState[i].bIsPreviousPush = true;		// ���� Tick�� ���� ����(���� Tick)�� �ش� Ű�� �����ٰ� ����
		}
		else	// Ű�� ������ ���� ���
		{
			if (true == m_tKeyInputState[i].bIsPreviousPush)
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_UP;		// ������ �������� ����� ������ ���� ��� -> KeyUp
			}
			else
			{
				m_tKeyInputState[i].eState = INPUTSTATE::IS_NONE;	// ������ �� �������� ���絵 ������ ���� ��� -> KeyNone
			}

			m_tKeyInputState[i].bIsPreviousPush = false;	// ���� Tick�� ���� ����(���� Tick)�� �ش� Ű�� ������ �ʾҴٰ� ����
		}
	}

	// MouseInput
	for (_uint i = 0; i < (_uint)MOUSEKEYSTATE::DIMK_END; ++i)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)	// ���콺 Ŭ���� ���
		{
			if (true == m_tMouseInputState[i].bIsPreviousPush)
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_STAY;		// ������ Ŭ���߾��� ���絵 Ŭ���� ��� -> MouseStay
			}
			else
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_DOWN;		// ������ Ŭ�� ���߾��� ����� Ŭ���� ��� -> MouseDown
			}

			m_tMouseInputState[i].bIsPreviousPush = true;	// ���� Tick�� ���� ����(���� Tick)�� Ŭ���߾��ٰ� ����
		}
		else	// Ŭ�� �� �� ���
		{
			if (true == m_tMouseInputState[i].bIsPreviousPush)
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_UP;		// ������ Ŭ�� �� �߾��� ����� Ŭ���� ��� -> MouseUp
			}
			else
			{
				m_tMouseInputState[i].eState = INPUTSTATE::IS_NONE;		// ������ Ŭ�� �� �߾��� ���絵 Ŭ�� �� �� ��� -> MouseNone
			}

			m_tMouseInputState[i].bIsPreviousPush = false;	// ���� Tick�� ���� ����(���� Tick)�� Ŭ�� �� �߾��ٰ� ����
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
