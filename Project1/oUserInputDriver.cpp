#include "oUserInputDriver.h"
#include "jGameObjectMgr.h"
#include "ObjTerrainMgr.h"
#include "oCollisionMgr.h"
#include "ObjCamera.h"
#include "cCollider.h"
#include "jMesh.h"
#include "jShaderColor.h"
#include "jGameObject.h"
#include "cUserInputDriven.h"
#include "jInput.h"

oUserInputDriver::oUserInputDriver()
{
}


oUserInputDriver::~oUserInputDriver()
{
}

void oUserInputDriver::OnLoad()
{
	jMesh* mesh = new jMesh();
	mesh->LoadCubeOutline(1);
	AddComponent(mesh);
	AddComponent(new jShaderColor());
}

void oUserInputDriver::OnStart()
{
	mCamera = GetEngine().FindGameObject<ObjCamera>();
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	mCollision = GetEngine().FindGameObject<oCollisionMgr>();
}

void oUserInputDriver::OnUpdate()
{
	jInput& inst = jInput::GetInst();
	list<pair<DetectType, int>>& inputs = inst.GetDetectedInputs();

	Vector2n mousePt = inst.GetMousePoint();
	Vector3 view = mCamera->ScreenToWorldView(mousePt.x, mousePt.y);
	Vector3 camPos = mCamera->GetTransform().getPos();
	mTerrain->RayCastTerrain(camPos, view, mPositionOnGround);
	CrashResult ret = mCollision->RayCast(camPos, mPositionOnGround);

	InputEventArgs args;
	args.pt = mousePt;
	args.deltaPt = inst.GetMouseDelta();
	args.terrainPt = mPositionOnGround;
	args.hoveredObject = ret.isCrash ? ret.from->GetGameObject() : nullptr;
	for (auto iter : inputs)
	{
		switch (iter.first)
		{
		case DetectType::MouseDown:
			mMouseDownType = ToMouseButtonType(iter.second);
			mMouseDraged = false;
			args.type = mMouseDownType;
			InvokeMouseDown(args);
			break;
		case DetectType::MouseUp:
			mMouseDownType = MouseButtonType::None;
			args.type = ToMouseButtonType(iter.second);
			InvokeMouseUp(args);
			if (!mMouseDraged)
			{
				args.type = ToMouseButtonType(iter.second);
				InvokeMouseClick(args);
			}
			mMouseDraged = false;
			break;
		case DetectType::MouseMove:
			if (MouseButtonType::None != mMouseDownType)
			{
				mMouseDraged = true;
				args.type = mMouseDownType;
				InvokeMouseDrag(args);
			}
			else
			{
				InvokeMouseMove(args);
			}
			break;
		case DetectType::MouseWheel:
			args.delta = iter.second;
			InvokeMouseWheel(args);
			break;
		case DetectType::KeyDown:
			mKeyPressed[iter.second] = 1;
			args.key = iter.second;
			InvokeKeyDown(args);
			break;
		case DetectType::KeyUp:
			mKeyPressed.erase(iter.second);
			args.key = iter.second;
			InvokeKeyUp(args);
			break;
		default:
			break;
		}
	}

	for (auto iter : mKeyPressed)
	{
		args.key = iter.first;
		InvokeKeyPressed(args);
	}

	ShowColliderBox(args.hoveredObject);
}

void oUserInputDriver::InvokeMouseDown(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseDown)
		{
			if (EventResult::StopEvent == driven->EventMouseDown(args))
				break;
		}
	}
}

void oUserInputDriver::InvokeMouseUp(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseUp)
		{
			if (EventResult::StopEvent == driven->EventMouseUp(args))
				break;
		}
	}
}

void oUserInputDriver::InvokeMouseMove(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseMove)
		{
			if (EventResult::StopEvent == driven->EventMouseMove(args))
				break;
		}
	}

	//if (nullptr != args.hoveredObject)
	//	HoverEnter(args.hoveredObject);
	//else
	//	HoverLeave();
}

void oUserInputDriver::InvokeMouseClick(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseClick)
		{
			if (EventResult::StopEvent == driven->EventMouseClick(args))
				break;
		}
	}

	//if (nullptr == mHoveredObj)
	//	UnSelectObj();
	//else
	//	SelectObj(mHoveredObj);
}

void oUserInputDriver::InvokeMouseDrag(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseDrag)
		{
			if (EventResult::StopEvent == driven->EventMouseDrag(args))
				break;
		}
	}
}

void oUserInputDriver::InvokeMouseWheel(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventMouseWheel)
		{
			if (EventResult::StopEvent == driven->EventMouseWheel(args))
				break;
		}
	}
}

void oUserInputDriver::InvokeKeyDown(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventKeyDown)
		{
			if (EventResult::StopEvent == driven->EventKeyDown(args))
				break;
		}
	}

	if (nullptr == mSelectedObj)
		return;

	if (args.key == 0x60) //ESC Pressed
	{
		UnSelectObj();
		return;
	}

	cUserInputDriven* driven = mSelectedObj->FindComponent<cUserInputDriven>();
	if (nullptr != driven && driven->EventKeyDownObject)
		driven->EventKeyDownObject(args.key);
}

void oUserInputDriver::InvokeKeyUp(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventKeyUp)
		{
			if (EventResult::StopEvent == driven->EventKeyUp(args))
				break;
		}
	}

}

void oUserInputDriver::InvokeKeyPressed(InputEventArgs args)
{
	for (auto item : mDrivens)
	{
		cUserInputDriven* driven = item.second;
		if (driven->EventKeyPressed)
		{
			if (EventResult::StopEvent == driven->EventKeyPressed(args))
				break;
		}
	}
}

void oUserInputDriver::HoverEnter(jGameObject * obj)
{
	if (mHoveredObj == obj)
		return;

	if (mHoveredObj != obj)
		HoverLeave();

	mHoveredObj = obj;
	cUserInputDriven* driven = mHoveredObj->FindComponent<cUserInputDriven>();
	if (nullptr != driven && driven->EventHoverEnter)
		driven->EventHoverEnter();
}

void oUserInputDriver::HoverLeave()
{
	if (nullptr == mHoveredObj)
		return;

	cUserInputDriven* driven = mHoveredObj->FindComponent<cUserInputDriven>();
	if (nullptr != driven && driven->EventHoverLeave)
		driven->EventHoverLeave();
	mHoveredObj = nullptr;
}

void oUserInputDriver::SelectObj(jGameObject * obj)
{
	if (mSelectedObj == obj)
	{
		UnSelectObj();
		return;
	}

	if (mSelectedObj != obj)
		UnSelectObj();

	mSelectedObj = obj;
	cUserInputDriven* driven = mSelectedObj->FindComponent<cUserInputDriven>();
	if (nullptr != driven && driven->EventSelected)
		driven->EventSelected();
}

void oUserInputDriver::UnSelectObj()
{
	if (nullptr == mSelectedObj)
		return;

	cUserInputDriven* driven = mSelectedObj->FindComponent<cUserInputDriven>();
	if (nullptr != driven && driven->EventUnSelected)
		driven->EventUnSelected();
	mSelectedObj = nullptr;
}

void oUserInputDriver::ShowColliderBox(jGameObject* obj)
{
	if (nullptr == obj)
	{
		GetTransform().moveTo(Vector3());
		GetTransform().Zoom(Vector3());
		return;
	}
	cCollider* col = obj->FindComponent<cCollider>();
	if (nullptr == col)
	{
		GetTransform().moveTo(Vector3());
		GetTransform().Zoom(Vector3());
		return;
	}

	jRect3D rect = col->GetShape()->GetBox();
	GetTransform().moveTo(rect.Center());
	GetTransform().Zoom(rect.Size());
}

MouseButtonType oUserInputDriver::ToMouseButtonType(int type)
{
	switch (type)
	{
	case 0: return MouseButtonType::Left;
	case 1: return MouseButtonType::Right;
	case 2: return MouseButtonType::Center;
	default: _warn(); break;
	}
	return MouseButtonType::None;
}
