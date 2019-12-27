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
	// 마우스 커서의 위치 지정에 사용될 화면 크기를 설정합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Direct Input 인터페이스를 초기화 합니다.
	HRESULT result = DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드의 Direct Input 인터페이스를 생성합니다
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식을 설정하십시오. 이 경우 키보드이므로 사전 정의 된 데이터 형식을 사용할 수 있습니다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// 다른 프로그램과 공유하지 않도록 키보드의 협조 수준을 설정합니다
	result = m_keyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 Direct Input 인터페이스를 생성합니다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 미리 정의 된 마우스 데이터 형식을 사용하여 마우스의 데이터 형식을 설정합니다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// 다른 프로그램과 공유 할 수 있도록 마우스의 협력 수준을 설정합니다.
	result = m_mouse->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 할당받는다
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스를 할당받는다
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void jInput::Release()
{
	// 마우스를 반환합니다.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// 키보드를 반환합니다.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// m_directInput 객체를 반환합니다.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool jInput::Update()
{
	HRESULT result;
	// 키보드 디바이스를 얻는다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// 키보드가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져 온다
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// 마우스 디바이스를 얻는다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// 마우스가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져 온다
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