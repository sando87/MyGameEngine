#include "jInput.h"
#include "jOS_APIs.h"


jInput::jInput()
{
	mToAscii[DIK_0] = '0';	mToAscii[DIK_1] = '1';	mToAscii[DIK_2] = '2';	mToAscii[DIK_3] = '3';	mToAscii[DIK_4] = '4';	
	mToAscii[DIK_5] = '5';	mToAscii[DIK_6] = '6';	mToAscii[DIK_7] = '7';	mToAscii[DIK_8] = '8';	mToAscii[DIK_9] = '9';

	mToAscii[DIK_A] = 'A';	mToAscii[DIK_B] = 'B';	mToAscii[DIK_C] = 'C';	mToAscii[DIK_D] = 'D';	mToAscii[DIK_E] = 'E';
	mToAscii[DIK_F] = 'F';	mToAscii[DIK_G] = 'G';	mToAscii[DIK_H] = 'H';	mToAscii[DIK_I] = 'I';	mToAscii[DIK_J] = 'J';
	mToAscii[DIK_K] = 'K';	mToAscii[DIK_L] = 'L';	mToAscii[DIK_M] = 'M';	mToAscii[DIK_N] = 'N';	mToAscii[DIK_O] = 'O';
	mToAscii[DIK_P] = 'P';	mToAscii[DIK_Q] = 'Q';	mToAscii[DIK_R] = 'R';	mToAscii[DIK_S] = 'S';	mToAscii[DIK_T] = 'T';
	mToAscii[DIK_U] = 'U';	mToAscii[DIK_V] = 'V';	mToAscii[DIK_W] = 'W';	mToAscii[DIK_X] = 'X';	mToAscii[DIK_Y] = 'Y';
	mToAscii[DIK_Z] = 'Z';

	mToAscii[DIK_BACK] = '\b';
	mToAscii[DIK_TAB] = '\t';
	mToAscii[DIK_ESCAPE] = '`';
	mToAscii[DIK_GRAVE] = '\n';
	mToAscii[DIK_SPACE] = ' ';


	mToAscii[DIK_SLASH] = '/';
	mToAscii[DIK_MULTIPLY] = '*';
	mToAscii[DIK_SUBTRACT] = '-';
	mToAscii[DIK_ADD] = '+';

}


jInput::~jInput()
{
}

bool jInput::Initialize(HINSTANCE _hInst, HWND _hWnd, int screenWidth, int screenHeight)
{
	// 마우스 커서의 위치 지정에 사용될 화면 크기를 설정합니다.
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Direct Input 인터페이스를 초기화 합니다.
	HRESULT result = DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDirectInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드의 Direct Input 인터페이스를 생성합니다
	result = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboardDev, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식을 설정하십시오. 이 경우 키보드이므로 사전 정의 된 데이터 형식을 사용할 수 있습니다.
	result = mKeyboardDev->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// 다른 프로그램과 공유하지 않도록 키보드의 협조 수준을 설정합니다
	result = mKeyboardDev->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 Direct Input 인터페이스를 생성합니다.
	result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouseDev, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 미리 정의 된 마우스 데이터 형식을 사용하여 마우스의 데이터 형식을 설정합니다.
	result = mMouseDev->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// 다른 프로그램과 공유 할 수 있도록 마우스의 협력 수준을 설정합니다.
	result = mMouseDev->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 할당받는다
	result = mKeyboardDev->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스를 할당받는다
	result = mMouseDev->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void jInput::Release()
{
	// 마우스를 반환합니다.
	if (mMouseDev)
	{
		mMouseDev->Unacquire();
		mMouseDev->Release();
		mMouseDev = nullptr;
	}

	// 키보드를 반환합니다.
	if (mKeyboardDev)
	{
		mKeyboardDev->Unacquire();
		mKeyboardDev->Release();
		mKeyboardDev = nullptr;
	}

	// m_directInput 객체를 반환합니다.
	if (mDirectInput)
	{
		mDirectInput->Release();
		mDirectInput = nullptr;
	}
}

bool jInput::Update()
{
	HRESULT result;
	// 키보드 디바이스를 얻는다.
	result = mKeyboardDev->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);
	if (FAILED(result))
	{
		// 키보드가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져 온다
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mKeyboardDev->Acquire();
		}
		else
		{
			return false;
		}
	}

	// 마우스 디바이스를 얻는다.
	result = mMouseDev->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseState);
	if (FAILED(result))
	{
		// 마우스가 포커스를 잃었거나 획득되지 않은 경우 컨트롤을 다시 가져 온다
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mMouseDev->Acquire();
		}
		else
		{
			return false;
		}
	}

	DetectInputs();

	mMousePrevState = mMouseState;
	memcpy(mKeyboardPrevState, mKeyboardState, sizeof(char) * 256);
	mMousePosition = jOS_APIs::GetCursorScreenPos();
	
	CallEvents();

	return true;
}

void jInput::DetectInputs()
{
	mDetectedInputs.clear();

	for (auto iter : mToAscii)
	{
		int idx = iter.first;
		if (mKeyboardPrevState[idx] == 0 && mKeyboardState[idx] == 0x80)
			mDetectedInputs.push_back(make_pair(DetectType::KeyDown, iter.second));
		else if (mKeyboardPrevState[idx] == 0x80 && mKeyboardState[idx] == 0)
			mDetectedInputs.push_back(make_pair(DetectType::KeyUp, iter.second));
	}

	if(mMouseState.lX != 0 || mMouseState.lY != 0)
		mDetectedInputs.push_back(make_pair(DetectType::MouseMove, 0));

	if (mMouseState.lZ != 0)
		mDetectedInputs.push_back(make_pair(DetectType::MouseWheel, mMouseState.lZ));

	for (int i = 0; i < 4; ++i)
	{
		if(mMousePrevState.rgbButtons[i] == 0 && mMouseState.rgbButtons[i] == 0x80)
			mDetectedInputs.push_back(make_pair(DetectType::MouseDown, i));
		else if (mMousePrevState.rgbButtons[i] == 0x80 && mMouseState.rgbButtons[i] == 0)
			mDetectedInputs.push_back(make_pair(DetectType::MouseUp, i));
	}
}

void jInput::CallEvents()
{
	for (auto iter : mDetectedInputs)
	{
		switch (iter.first)
		{
		case DetectType::MouseDown:
			MouseDown(mMousePosition, iter.second);
			break;
		case DetectType::MouseUp:
			MouseUp(mMousePosition, iter.second);
			break;
		case DetectType::MouseMove:
			MouseMove(mMousePosition, GetMouseDelta());
			break;
		case DetectType::MouseWheel:
			MouseWheel(iter.second);
			break;
		case DetectType::KeyDown:
			KeyDown(iter.second);
			break;
		case DetectType::KeyUp:
			KeyUp(iter.second);
			break;
		default:
			break;
		}
	}
}
