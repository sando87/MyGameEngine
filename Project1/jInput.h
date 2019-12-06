#ifndef __JINPUT_H__
#define __JINPUT_H__

#include "header.h"
#include <functional>
#include "jFuncPtrList.h"

class jInput
{
public:
	static jInput& GetInst() { static jInput inst; return inst; }
	typedef struct _jMouseInfo {
		int    x;
		int    y;
		int    z;
		unsigned char   left;
		unsigned char   right;
		unsigned char   middle;
		unsigned char   reserve;
		int    fixedX;
		int    fixedY;
	} jMouseInfo;

private:
	jInput();
	~jInput();
public:
	bool Initialize(HINSTANCE, HWND, int, int);
	void Release();
	bool Update();

	jFuncPtrList<void(jMouseInfo)> mMouse;
	jFuncPtrList<void(jMouseInfo)> mMouseDown;
	//std::function<void(jMouseInfo)> mMouse;
	std::function<void(const unsigned char*)> mKeyboard;

private:
	IDirectInput8 * m_directInput = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;

	DIMOUSESTATE mMousePreviousInfo;
	unsigned char m_keyboardState[256] = { 0, };
	DIMOUSESTATE m_mouseState;
	bool isKeyIN();
	bool isMouseIN();
	bool isMouseDown();
	int m_screenWidth = 0;
	int m_screenHeight = 0;
};

#endif