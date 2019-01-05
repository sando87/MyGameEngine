#include "jInput.h"
#include "jLog.h"


jInput::jInput()
{
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

	if (m_mouseState.rgbButtons[0] > 0)
	{
		if(mFunc != nullptr)
			mFunc(m_mouseState.lX, m_mouseState.lY);
	}

	return true;
}
