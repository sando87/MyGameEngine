#include "jInputEvent.h"
#include "jInput.h"


jInputEvent::jInputEvent()
{
}


jInputEvent::~jInputEvent()
{
}

void jInputEvent::OnUpdate()
{
	jInput& inst = jInput::GetInst();
	list<pair<DetectType, int>>& inputs = inst.GetDetectedInputs();

	for (auto iter : inputs)
	{
		switch (iter.first)
		{
		case DetectType::MouseDown:
			mMouseDownType = iter.second;
			mMouseDraged = false;
			OnMouseDown(inst.GetMousePoint(), iter.second);
			break;
		case DetectType::MouseUp:
			mMouseDownType = -1;
			OnMouseUp(inst.GetMousePoint(), iter.second);
			if(!mMouseDraged)
				OnMouseClick(inst.GetMousePoint(), iter.second);
			mMouseDraged = false;
			break;
		case DetectType::MouseMove:
			if (mMouseDownType >= 0)
			{
				mMouseDraged = true;
				OnMouseDrag(inst.GetMouseDelta(), mMouseDownType);
			}
			OnMouseMove(inst.GetMousePoint(), inst.GetMouseDelta());
			break;
		case DetectType::MouseWheel:
			OnMouseWheel(iter.second);
			break;
		case DetectType::KeyDown:
			mKeyPressed[iter.second] = 1;
			OnKeyDown(iter.second);
			break;
		case DetectType::KeyUp:
			mKeyPressed.erase(iter.second);
			OnKeyUp(iter.second);
			break;
		default:
			break;
		}
	}

	for (auto iter : mKeyPressed)
		OnKeyPressed(iter.first);
}
