#ifndef __JINPUT_H__
#define __JINPUT_H__

#include "junks.h"
#include "jDirectXheader.h"

enum DetectType
{
	MouseUp, MouseDown, MouseMove, MouseWheel, KeyUp, KeyDown
};

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

	jFuncPtrList<void(Vector2n, int)> MouseUp;
	jFuncPtrList<void(Vector2n, int)> MouseDown;
	jFuncPtrList<void(Vector2n, Vector2n)> MouseMove;
	jFuncPtrList<void(int)> MouseWheel;
	jFuncPtrList<void(char)> KeyUp;
	jFuncPtrList<void(char)> KeyDown;

	list<pair<DetectType, int>>& GetDetectedInputs() { return mDetectedInputs; }
	Vector2n GetMouseDelta() { return Vector2n(mMouseState.lX, mMouseState.lY); }
	Vector2n GetMousePoint() { return mMousePosition; }

private:
	IDirectInput8 * mDirectInput = nullptr;
	IDirectInputDevice8* mKeyboardDev = nullptr;
	IDirectInputDevice8* mMouseDev = nullptr;

	unordered_map<char, char> mToAscii;
	DIMOUSESTATE mMousePrevState = { 0, };
	DIMOUSESTATE mMouseState = { 0, };
	unsigned char mKeyboardPrevState[256] = { 0, };
	unsigned char mKeyboardState[256] = { 0, };

	list<pair<DetectType, int>> mDetectedInputs;
	
	Vector2n mMousePosition;
	int mScreenWidth = 0;
	int mScreenHeight = 0;

	void DetectInputs();
	void CallEvents();
};

#endif