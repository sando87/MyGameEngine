#pragma once
#include "jComponent.h"

class jInputEvent :
	public jComponent
{
public:
	jInputEvent();
	virtual ~jInputEvent();

protected:
	virtual void OnUpdate();

	virtual void OnMouseDown(Vector2n pt, int type) {}
	virtual void OnMouseUp(Vector2n pt, int type) {}
	virtual void OnMouseMove(Vector2n pt, Vector2n delta) {}
	virtual void OnMouseWheel(int delta) {}
	virtual void OnMouseClick(Vector2n pt, int type) {}
	virtual void OnMouseDrag(Vector2n delta, int type) {}
	virtual void OnKeyDown(char ch) {}
	virtual void OnKeyUp(char ch) {}
	virtual void OnKeyPressed(char ch) {}

	unordered_map<char, int> mKeyPressed;
	Property_Getter(int, MouseDownType, -1)
	Property_Getter(bool, MouseDraged, false)
};

