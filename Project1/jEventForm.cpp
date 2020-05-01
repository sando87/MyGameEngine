#include "jEventForm.h"
#include <jUISystem.h>
#include <jView.h>

jEventForm::jEventForm(jUISystem *form, char toggleKey)
{
	mForm = form;
	mToggleKey = toggleKey;

	SetPriority(EventPriority::High);
	EventMouseDown = [&](InputEventArgs args) {
		if(GetGameObject()->GetEnable() == false)
			return EventResult::TransferEvent;

		jView* rootView = mForm->GetRootView();
		if(false == rootView->GetRectAbsolute().Contains(Point2(args.pt.x, args.pt.y)))
			return EventResult::TransferEvent;

		mForm->SetMouseEvent(Point2(args.pt.x, args.pt.y), true, true);
		return EventResult::StopEvent;
	};
	EventMouseUp = [&](InputEventArgs args) {
		if (GetGameObject()->GetEnable() == false)
			return EventResult::TransferEvent;

		jView* rootView = mForm->GetRootView();
		if (false == rootView->GetRectAbsolute().Contains(Point2(args.pt.x, args.pt.y)))
			return EventResult::TransferEvent;

		mForm->SetMouseEvent(Point2(args.pt.x, args.pt.y), false, true);
		return EventResult::StopEvent;
	};
	EventMouseMove = [&](InputEventArgs args) {
		if (GetGameObject()->GetEnable() == false)
			return EventResult::TransferEvent;

		jView* rootView = mForm->GetRootView();
		if (false == rootView->GetRectAbsolute().Contains(Point2(args.pt.x, args.pt.y)))
			return EventResult::TransferEvent;

		mForm->SetMouseEvent(Point2(args.pt.x, args.pt.y), false, false);
		return EventResult::StopEvent;
	};
	EventKeyDown = [&](InputEventArgs args) {
		if (args.key == mToggleKey)
		{
			bool curEnableState = GetGameObject()->GetEnable();
			GetGameObject()->SetEnable(!curEnableState);
		}
		return EventResult::TransferEvent;
	};
}


jEventForm::~jEventForm()
{
}
