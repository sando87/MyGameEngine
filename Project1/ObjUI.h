#pragma once
#include "jGameObject.h"
#include "jShaderHeader.h"
#include <jUISystem.h>


class jUISystem;
class jShaderUIEngine;
class jInputEvent;
class jInventory;
class jViewGrid;
class jView;
class jViewImage;
class ObjItem;

class ObjUI :
	public jGameObject
{
public:
	ObjUI();
	virtual ~ObjUI();

	function<void(ObjItem*, int)> EventMoveItem;
	function<void(ObjItem*)> EventEquipItem;
	function<void(ObjItem*)> EventUnEquipItem;

	bool AddItem(ObjItem* item);

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jUISystem * mUIEngine;
	jShaderUIEngine * mShader;
	jInputEvent * mEvent;

	jViewGrid* mViewGrid;
	jView* mViewSlots[10];
	jViewImage* mViewLight;
	jViewImage* mViewActive;
	jView* mSelectedView;
	double mCellWidth;
	double mCellHeight;

	void ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4]);
	void DoClickEvent(jView* clickedView);
	void DoHightlight(jView* hoveredView);
	void Reset();
};

