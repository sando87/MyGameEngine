#include "jEventForm.h"
#include <jUISystem.h>


jEventForm::jEventForm(jUISystem *form)
{
	mForm = form;
}


jEventForm::~jEventForm()
{
}

void jEventForm::OnMouseDown(Vector2n pt, int type)
{
	mForm->SetMouseEvent(Point2(pt.x, pt.y), true, true);
}

void jEventForm::OnMouseUp(Vector2n pt, int type)
{
	mForm->SetMouseEvent(Point2(pt.x, pt.y), false, true);
}

void jEventForm::OnMouseMove(Vector2n pt, Vector2n delta)
{
	mForm->SetMouseEvent(Point2(pt.x, pt.y), false, false);
}
