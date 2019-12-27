#include "jInput.h"


jInput::jInput()
{
	memset(&mMousePreviousInfo, 0x00, sizeof(mMousePreviousInfo));
}


jInput::~jInput()
{
}

bool jInput::Initialize(HINSTANCE _hInst, HWND _hWnd, int screenWidth, int screenHeight)
{
	// ���콺 Ŀ���� ��ġ ������ ���� ȭ�� ũ�⸦ �����մϴ�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Direct Input �������̽��� �ʱ�ȭ �մϴ�.
	HRESULT result = DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Ű������ Direct Input �������̽��� �����մϴ�
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ������ ������ �����Ͻʽÿ�. �� ��� Ű�����̹Ƿ� ���� ���� �� ������ ������ ����� �� �ֽ��ϴ�.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// �ٸ� ���α׷��� �������� �ʵ��� Ű������ ���� ������ �����մϴ�
	result = m_keyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// ���콺 Direct Input �������̽��� �����մϴ�.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// �̸� ���� �� ���콺 ������ ������ ����Ͽ� ���콺�� ������ ������ �����մϴ�.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// �ٸ� ���α׷��� ���� �� �� �ֵ��� ���콺�� ���� ������ �����մϴ�.
	result = m_mouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���带 �Ҵ�޴´�
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// ���콺�� �Ҵ�޴´�
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void jInput::Release()
{
	// ���콺�� ��ȯ�մϴ�.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Ű���带 ��ȯ�մϴ�.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// m_directInput ��ü�� ��ȯ�մϴ�.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool jInput::Update()
{
	HRESULT result;
	// Ű���� ����̽��� ��´�.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// Ű���尡 ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ���� �ٽ� ���� �´�
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// ���콺 ����̽��� ��´�.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// ���콺�� ��Ŀ���� �Ҿ��ų� ȹ����� ���� ��� ��Ʈ���� �ٽ� ���� �´�
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	if (isMouseIN())
	{
		jMouseInfo info = { 0, };
		memcpy(&info, &m_mouseState, sizeof(m_mouseState));
		mMouse(info);
	}

	if (isMouseDown())
	{
		jMouseInfo info = { 0, };
		memcpy(&info, &m_mouseState, sizeof(m_mouseState));
		mMouseDown(info);
	}
	
	if (isKeyIN())
	{
		mKeyboard(m_keyboardState);
	}

	memcpy(&mMousePreviousInfo, &m_mouseState, sizeof(m_mouseState));
	return true;
}
bool jInput::isKeyIN()
{
	int size = sizeof(m_keyboardState);
	for (int i = 0; i < size; i++)
		if (m_keyboardState[i] != 0)
			return true;
	return false;
}
bool jInput::isMouseIN()
{
	if (m_mouseState.lX == 0 &&
		m_mouseState.lY == 0 &&
		m_mouseState.lZ == 0 &&
		m_mouseState.rgbButtons[0] == 0 &&
		m_mouseState.rgbButtons[1] == 0 &&
		m_mouseState.rgbButtons[2] == 0 &&
		m_mouseState.rgbButtons[3] == 0)
		return false;
	return true;
}
bool jInput::isMouseDown()
{
	if (mMousePreviousInfo.rgbButtons[0] == 0 && m_mouseState.rgbButtons[0] == 0x80)
		return true;

	return false;
}