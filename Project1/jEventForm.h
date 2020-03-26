#pragma once
#include "jInputEvent.h"

class jUISystem;

class jEventForm :
	public jInputEvent
{
public:
	jEventForm(jUISystem *form);
	virtual ~jEventForm();

protected:
	jUISystem * mForm;

	virtual void OnMouseDown(Vector2n pt, int type);
	virtual void OnMouseUp(Vector2n pt, int type);
	virtual void OnMouseMove(Vector2n pt, Vector2n delta);
};

