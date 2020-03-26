#pragma once
#include "jGameObject.h"
#include "jShaderHeader.h"
#include <jUISystem.h>


class jUISystem;
class jInventory;
class jViewGrid;
class jView;
class jViewImage;
class ObjItem;
class jEventFormInven;
class ObjPlayer;
struct DBItem;

class ObjUI :
	public jGameObject
{
	friend class jEventFormInven;
public:
	ObjUI();
	virtual ~ObjUI();

	bool IsFull();
	bool PickItem(const DBItem& itemDB);
	jView* GetSelectedView() { return mSelectedItem; }

protected:
	virtual void OnLoad();
	virtual void OnStart();

	jUISystem * mUIForm;
	ObjPlayer * mPlayer;

	jViewGrid* mGridView;
	unordered_map<string, jView*> mSlotViews;
	jViewImage* mHoverOnForm;
	jViewImage* mActiveOnItem;
	jView* mSelectedItem;
	double mCellWidth;
	double mCellHeight;

	void MoveItem(u32 itemID, u32 pos);
	void Equip(u32 itemID);
	void UnEquip(u32 itemID, u32 pos);
	void DropItem(u32 itemID);

	jView* CreateItemView(u32 itemID);
	void DoClickGrid(jView* cell);
	void DoClickSlot(jView* slot);
	void DoHightlight(jView* hoveredView);
	void Reset();
};

