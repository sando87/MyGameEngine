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

class ObjUI :
	public jGameObject
{
public:
	ObjUI();
	virtual ~ObjUI();

	void UpdateItemViews();


protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	jUISystem * mUIEngine;
	jShaderUIEngine * mShader;
	jInputEvent * mEvent;
	jViewGrid* mViewGrid;
	jView* mViewSlots[10];

	void ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4]);
	void Reset();

	Property_Setter(jInventory*, Inventory, nullptr)
};

