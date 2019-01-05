#ifndef __JINPUT_H__
#define __JINPUT_H__

#include "header.h"
#include <functional>

class jInput
{
public:
	static jInput& GetInst() { static jInput inst; return inst; }
private:
	jInput();
	~jInput();
public:
	bool Initialize(HINSTANCE, HWND, int, int);
	void Release();
	bool Update();

	std::function<void(int, int)> mFunc;

private:
	IDirectInput8 * m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
};

#endif