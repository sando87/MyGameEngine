#pragma once
#include "jGameObject.h"
#include "junks.h"
#include "cUserInputDriven.h"

class ObjCamera;
class ObjTerrainMgr;
class oCollisionMgr;

class oUserInputDriver :
	public jGameObject
{
public:
	oUserInputDriver();
	~oUserInputDriver();

	void AddDriven(cUserInputDriven* driven) { mDrivens.insert(make_pair((int)driven->GetPriority(), driven)); }
protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	ObjCamera* mCamera;
	ObjTerrainMgr* mTerrain;
	oCollisionMgr* mCollision;

	bool mMouseDraged = false;
	jGameObject* mHoveredObj = nullptr;
	jGameObject* mSelectedObj = nullptr;
	MouseButtonType mMouseDownType = MouseButtonType::None;

	Vector3 mPositionOnGround;
	unordered_map<char, int> mKeyPressed;
	multimap<int, cUserInputDriven*> mDrivens;

	void InvokeMouseDown(InputEventArgs args);
	void InvokeMouseUp(InputEventArgs args);
	void InvokeMouseMove(InputEventArgs args);
	void InvokeMouseClick(InputEventArgs args);
	void InvokeMouseDrag(InputEventArgs args);
	void InvokeMouseWheel(InputEventArgs args);
	void InvokeKeyDown(InputEventArgs args);
	void InvokeKeyUp(InputEventArgs args);
	void InvokeKeyPressed(InputEventArgs args);

	void HoverEnter(jGameObject* obj);
	void HoverLeave();
	void SelectObj(jGameObject* obj);
	void UnSelectObj();
	void ShowColliderBox(jGameObject* obj);
	MouseButtonType ToMouseButtonType(int type);
};

