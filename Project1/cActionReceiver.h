#pragma once
#include "jComponent.h"
#include "junks.h"

class cActionReceiver :
	public jComponent
{
public:
	cActionReceiver();
	~cActionReceiver();

	void RegisterAction(function<void(jGameObject*)> action) { mEventAction = action; }
	void InvokeAction(jGameObject* obj) { if(mEventAction) mEventAction(obj); }

protected:
	function<void(jGameObject*)> mEventAction;

	Property_GetSetter(bool, Attackable, false)
};

