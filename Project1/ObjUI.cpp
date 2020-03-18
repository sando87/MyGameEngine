#include "ObjUI.h"
#include "jInputEvent.h"
#include "jShaderUI.h"
#include "jImage.h"
#include "jInventory.h"
#include "ObjItem.h"
#include "ObjPlayer.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "jHealthPoint.h"
#include "jTransform.h"

#include <jViewGrid.h>
#include <jViewImage.h>


#pragma comment(lib,"jGUI.lib")

#define FN_CellIndex	"Idx"
#define FN_Category		"Category"
#define FN_GameObject	"GameObject"

class jShaderUIEngine : public jShaderUI
{
private:
	jUISystem * mUIEngine;
	virtual void OnLoad() 
	{ 
		SetRenderOrder(6);
		mUIEngine = jUISystem::GetInst(); 
		jShaderUI::OnLoad();
	}
	virtual bool OnRender(ObjCamera* cam)
	{
		jShaderUI::ConfigRender();

		mUIEngine->Draw();
		
		return true;
	}
};
class jEventUI : public jInputEvent
{
private:
	jUISystem * mUIEngine;
	virtual void OnLoad() { mUIEngine = jUISystem::GetInst(); }
	virtual void OnMouseDown(Vector2n pt, int type) { mUIEngine->SetMouseEvent(Point2(pt.x, pt.y), true, true); }
	virtual void OnMouseUp(Vector2n pt, int type) { mUIEngine->SetMouseEvent(Point2(pt.x, pt.y), false, true); }
	virtual void OnMouseMove(Vector2n pt, Vector2n delta) { mUIEngine->SetMouseEvent(Point2(pt.x, pt.y), false, false); }
	virtual void OnMouseClick(Vector2n pt, int type);
};


ObjUI::ObjUI()
{
}


ObjUI::~ObjUI()
{
}


void ObjUI::OnLoad()
{
	mShader = new jShaderUIEngine;
	AddComponent(mShader);

	mEvent = new jEventUI();
	AddComponent(mEvent);

	mUIEngine = jUISystem::GetInst();
	mUIEngine->SetResourcePath("./res/ui/");
	mUIEngine->EventDrawFill = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, nullptr);
	};
	mUIEngine->EventDrawOutline = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, nullptr);
	};
	mUIEngine->EventDrawTexture = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, params.texture);
	};
	mUIEngine->OpLoadTexture = [&](jUIBitmap* bitmap) {
		void* ptr = nullptr;
		if (bitmap->fullname.length() > 0)
		{
			jImage img(bitmap->fullname);
			img.LoadImgFile();
			chars buf = img.GetBuffer();
			ptr = mShader->LoadTextureRes((unsigned char*)&buf[0], img.GetWidth(), img.GetHeight());

		}
		else if(!bitmap->buf.empty())
		{
			ptr = mShader->LoadTextureRes((unsigned char*)&bitmap->buf[0], bitmap->width, bitmap->height);
		}
		
		return ptr;
	};
	mUIEngine->OpReleaseTexture = [&](void* ptr) {
		mShader->ReleaseTextureRes(ptr);
	};
	mUIEngine->ParseJson("panelInven.json");
	mUIEngine->LoadViews();
	

	mGridView = (jViewGrid*)mUIEngine->FindView("InvenGrid");
	mGridView->UserData[FN_Category] = "null";
	mSlotViews["armor"] = mUIEngine->FindView("armor");
	mSlotViews["armor"]->UserData[FN_Category] = "armor";
	mSlotViews["gloves"] = mUIEngine->FindView("gloves");
	mSlotViews["gloves"]->UserData[FN_Category] = "gloves";
	mCellWidth = mGridView->GetWidth() / mGridView->ColumnCount;
	mCellHeight = mGridView->GetHeight() / mGridView->RowCount;


	mGridView->EventGridEnter = [&](jView* view, Point2 pt){ DoHightlight(view);};
	mGridView->EventGridLeave = [&](jView* view, Point2 pt) { mHoverOnForm->Detach(); };
	mGridView->EventGridClick = [&](jView* view, Point2 pt) { DoClickGrid(view); };
	mSlotViews["armor"]->EventMouseEnter = [&](jView* view, Point2 pt) { DoHightlight(view); };
	mSlotViews["armor"]->EventMouseLeave = [&](jView* view, Point2 pt) { mHoverOnForm->Detach(); };
	mSlotViews["armor"]->EventMouseClick = [&](jView* view, Point2 pt) { DoClickSlot(view); };
	mSlotViews["gloves"]->EventMouseEnter = [&](jView* view, Point2 pt) { DoHightlight(view); };
	mSlotViews["gloves"]->EventMouseLeave = [&](jView* view, Point2 pt) { mHoverOnForm->Detach(); };
	mSlotViews["gloves"]->EventMouseClick = [&](jView* view, Point2 pt) { DoClickSlot(view); };


	mActiveOnItem = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	mActiveOnItem->Width = "1.0";
	mActiveOnItem->Height = "1.0";
	mActiveOnItem->Image.filename = "itemgrade2.png";
	mActiveOnItem->Image.top = 0;
	mActiveOnItem->Image.bottom = 1.0f;
	mActiveOnItem->Image.left = 0.125f * 5;
	mActiveOnItem->Image.right = 0.125f * 6;
	mActiveOnItem->Enable = false;

	mHoverOnForm = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	mHoverOnForm->Width = "1.0";
	mHoverOnForm->Height = "1.0";
	mHoverOnForm->Image.filename = "itemgrade2.png";
	mHoverOnForm->Image.top = 0;
	mHoverOnForm->Image.bottom = 1.0f;
	mHoverOnForm->Image.left = 0.125f * 5;
	mHoverOnForm->Image.right = 0.125f * 6;
	mHoverOnForm->Enable = false;
}

void ObjUI::OnStart()
{
	mPlayer = GetEngine().FindGameObject<ObjPlayer>();
	u32 playID = mPlayer->GetID();
	vector<DBItem> items;
	jTinyDB::GetInst().ReadRecords(items, [playID](TinyRecord* rec) {
		return rec->GetValue<u32>("owner") == playID;
	});

	for (DBItem& item : items)
	{
		jView* newItemView = CreateItemView(item.GetID());
		if (item.state != ItemState::Equipped)
		{
			jView* child = mGridView->GetChild(item.posIndex);
			child->AddChild(newItemView);
			newItemView->LoadAll();
		}
		else
		{
			DBItemResource itemRes;
			itemRes.Load(item.rsrcID);
			mSlotViews[itemRes.category]->AddChild(newItemView);
			newItemView->LoadAll();
			Equip(item.GetID());
		}
	}
}

void ObjUI::OnUpdate()
{
	mUIEngine->MouseEventCall();
}

void ObjUI::MoveItem(u32 itemID, u32 pos)
{
	DBItem DBItem;
	DBItem.Load(itemID);
	DBItem.posIndex = pos;
	DBItem.Save();
}

void ObjUI::Equip(u32 itemID)
{
	DBItem item;
	item.Load(itemID);
	if (item.state != ItemState::Equipped)
	{
		item.state = ItemState::Equipped;
		item.Save();
	}

	DBItemResource resInfo;
	resInfo.Load(item.rsrcID);
	if (resInfo.category == "armor")
	{
		jGameObject* body = mPlayer->FindGameObject("body");
		jMesh* compMesh = body->FindComponent<jMesh>();
		jImage* compImg = body->FindComponent<jImage>();
		jShaderSkin* compshader = body->FindComponent<jShaderSkin>();
		compMesh->SetFullname(PATH_RESOURCES + string("mesh/") + resInfo.equipMesh);
		compImg->SetFullname(PATH_RESOURCES + string("img/") + resInfo.equipImg);
		compshader->UpdateInputResource();
	}
	else if (resInfo.category == "pants")
	{
	}
	else if (resInfo.category == "gloves")
	{
	}
	else if (resInfo.category == "shoes")
	{
	}

	jHealthPoint* stats = mPlayer->FindComponent<jHealthPoint>();
	if (stats != nullptr)
		stats->AddEffect(item);
}

void ObjUI::UnEquip(u32 itemID, u32 pos)
{
	DBItem item;
	item.Load(itemID);
	item.state = ItemState::Carraied;
	item.posIndex = pos;
	item.Save();

	DBItemResource resInfo;
	resInfo.Load(item.rsrcID);
	DBPlayer player;
	player.Load(mPlayer->GetID());
	DBClasses playerClass;
	playerClass.Load(player.classes);
	if (resInfo.category == "armor")
	{
		jGameObject* body = mPlayer->FindGameObject("body");
		jMesh* compMesh = body->FindComponent<jMesh>();
		jImage* compImg = body->FindComponent<jImage>();
		jShaderSkin* compshader = body->FindComponent<jShaderSkin>();
		compMesh->SetFullname(PATH_RESOURCES + string("mesh/") + playerClass.bodyMesh);
		compImg->SetFullname(PATH_RESOURCES + string("img/") + playerClass.bodyImg);
		compshader->UpdateInputResource();
	}
	else if (resInfo.category == "pants")
	{
	}
	else if (resInfo.category == "gloves")
	{
	}
	else if (resInfo.category == "shoes")
	{
	}

	jHealthPoint* stats = mPlayer->FindComponent<jHealthPoint>();
	if (stats != nullptr)
		stats->SubEffect(item);
}

void ObjUI::DropItem(u32 itemID)
{
	DBItem DBItem;
	DBItem.Load(itemID);
	DBItem.owner = 0;
	DBItem.posIndex = 0;
	DBItem.state = ItemState::Dropped;
	DBItem.Save();

	ObjItem* objitem = new ObjItem(itemID);
	Vector3 dropPos = mPlayer->GetTransform().getPos();
	objitem->GetTransform().moveTo(dropPos);
	GetEngine().AddGameObject(objitem);
}

jView * ObjUI::CreateItemView(u32 itemID)
{
	DBItem itemInfo;
	itemInfo.Load(itemID);
	DBItemResource itemResInfo;
	itemResInfo.Load(itemInfo.rsrcID);

	jViewImage* viewbg = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	viewbg->Width = "1.0";
	viewbg->Height = "1.0";
	viewbg->Image.filename = "itemgrade2.png";
	viewbg->Image.top = 0;
	viewbg->Image.bottom = 1;
	viewbg->Image.left = 0.125 * itemInfo.grade;
	viewbg->Image.right = 0.125 * (itemInfo.grade + 1);
	viewbg->Enable = false;
	viewbg->UserData[FN_GameObject] = itemID;
	viewbg->UserData[FN_Category] = itemResInfo.category;

	jViewImage* viewfg = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	viewfg->Width = "1.0";
	viewfg->Height = "1.0";
	viewfg->Image.filename = itemResInfo.uiImg;
	viewfg->Image.top = itemResInfo.Top();
	viewfg->Image.bottom = itemResInfo.Bottom();
	viewfg->Image.left = itemResInfo.Left();
	viewfg->Image.right = itemResInfo.Right();
	viewfg->Enable = false;
	viewfg->UserData[FN_GameObject] = itemID;
	viewfg->UserData[FN_Category] = itemResInfo.category;

	viewbg->AddChild(viewfg);
	return viewbg;
}

void ObjUI::ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4])
{
	vert[0].p = Vector3f(params.rect.Left(), params.rect.Top(), 0);
	vert[1].p = Vector3f(params.rect.Left(), params.rect.Bottom(), 0);
	vert[2].p = Vector3f(params.rect.Right(), params.rect.Top(), 0);
	vert[3].p = Vector3f(params.rect.Right(), params.rect.Bottom(), 0);

	vert[0].t = Vector2f(params.uv.Left(), params.uv.Top());
	vert[1].t = Vector2f(params.uv.Left(), params.uv.Bottom());
	vert[2].t = Vector2f(params.uv.Right(), params.uv.Top());
	vert[3].t = Vector2f(params.uv.Right(), params.uv.Bottom());

	Vector4f color = Vector4f(params.color.r, params.color.g, params.color.b, params.color.a) / 255.0f;
	vert[0].c = color;
	vert[1].c = color;
	vert[2].c = color;
	vert[3].c = color;
}

void ObjUI::DoClickGrid(jView* cell)
{
	jView* itemOnCell = cell->Childs.empty() ? nullptr : cell->Childs.front();
	if (itemOnCell == nullptr && mSelectedItem == nullptr)
		return;

	if (mSelectedItem != nullptr && itemOnCell != nullptr && mSelectedItem->GetParent()->GetParent() != mGridView)
	{
		string cateA = mSelectedItem->UserData[FN_Category].val<string>();
		string cateB = itemOnCell->UserData[FN_Category].val<string>();
		if (cateA != cateB)
			return;
	}

	if (itemOnCell == nullptr)
	{
		//MoveView
		bool isEquippedItem = mSelectedItem->GetParent()->GetParent() != mGridView;
		if (isEquippedItem)
		{
			u32 selItemID = mSelectedItem->UserData[FN_GameObject].val<u32>();
			int gridPosIdx = cell->UserData[FN_CellIndex].val<int>();
			UnEquip(selItemID, gridPosIdx);
		}
		else
		{
			u32 selItemID = mSelectedItem->UserData[FN_GameObject].val<u32>();
			int gridPosIdx = cell->UserData[FN_CellIndex].val<int>();
			MoveItem(selItemID, gridPosIdx);
		}

		mSelectedItem->Detach();
		cell->AddChild(mSelectedItem);
		mSelectedItem->LoadAll();
		mActiveOnItem->Detach();
		mSelectedItem = nullptr;

	}
	else if (mSelectedItem == nullptr)
	{
		//SelectView
		mSelectedItem = itemOnCell;
		mSelectedItem->AddChild(mActiveOnItem);
		mActiveOnItem->LoadAll();
	}
	else if (mSelectedItem == itemOnCell)
	{
		//UnSelectView
		mActiveOnItem->Detach();
		mSelectedItem = nullptr;
	}
	else
	{
		//SwapView
		jView* backView = mSelectedItem->GetParent();
		bool isEquippedItem = mSelectedItem->GetParent()->GetParent() != mGridView;
		if (isEquippedItem)
		{
			u32 unequipItemID = mSelectedItem->UserData[FN_GameObject].val<u32>();
			int gridPosIdx = cell->UserData[FN_CellIndex].val<int>();
			UnEquip(unequipItemID, gridPosIdx);

			u32 equipItemID = itemOnCell->UserData[FN_GameObject].val<u32>();
			Equip(equipItemID);
		}
		else
		{
			u32 itemA = mSelectedItem->UserData[FN_GameObject].val<u32>();
			int gridPosIdx = cell->UserData[FN_CellIndex].val<int>();
			MoveItem(itemA, gridPosIdx);

			u32 itemB = itemOnCell->UserData[FN_GameObject].val<u32>();
			gridPosIdx = backView->UserData[FN_CellIndex].val<int>();
			MoveItem(itemB, gridPosIdx);
		}

		mSelectedItem->Detach();
		itemOnCell->Detach();
		backView->AddChild(itemOnCell);
		cell->AddChild(mSelectedItem);
		mSelectedItem->LoadAll();
		itemOnCell->LoadAll();
		mActiveOnItem->Detach();
		mSelectedItem = nullptr;
	}
}

void ObjUI::DoClickSlot(jView* slot)
{
	jView* itemOnSlot = slot->Childs.empty() ? nullptr : slot->Childs.front();
	if (itemOnSlot == nullptr && mSelectedItem == nullptr)
		return;

	if (mSelectedItem != nullptr)
	{
		string cateA = mSelectedItem->UserData[FN_Category].val<string>();
		string cateB = slot->UserData[FN_Category].val<string>();
		if (cateA != cateB)
			return;
	}

	if (itemOnSlot == nullptr)
	{
		//MoveView
		mSelectedItem->Detach();
		slot->AddChild(mSelectedItem);
		mSelectedItem->LoadAll();

		u32 selectedItemID = mSelectedItem->UserData[FN_GameObject].val<u32>();
		Equip(selectedItemID);

		mActiveOnItem->Detach();
		mSelectedItem = nullptr;

	}
	else if (mSelectedItem == nullptr)
	{
		//SelectView
		mSelectedItem = itemOnSlot;
		mSelectedItem->AddChild(mActiveOnItem);
		mActiveOnItem->LoadAll();
	}
	else if (mSelectedItem == itemOnSlot)
	{
		//UnSelectView
		mActiveOnItem->Detach();
		mSelectedItem = nullptr;
	}
	else
	{
		//SwapView
		jView* cell = mSelectedItem->GetParent();
		mSelectedItem->Detach();
		itemOnSlot->Detach();
		cell->AddChild(itemOnSlot);
		slot->AddChild(mSelectedItem);
		mSelectedItem->LoadAll();
		itemOnSlot->LoadAll();

		u32 slotItemID = itemOnSlot->UserData[FN_GameObject].val<u32>();
		int gridPosIdx = cell->UserData[FN_CellIndex].val<int>();
		UnEquip(slotItemID, gridPosIdx);

		u32 selectedItemID = mSelectedItem->UserData[FN_GameObject].val<u32>();
		Equip(selectedItemID);

		mActiveOnItem->Detach();
		mSelectedItem = nullptr;
	}
}

void ObjUI::DoHightlight(jView * hoveredView)
{
	jView* parentView = hoveredView->GetParent();
	Point2 pos = hoveredView->GetLocalPos();
	mHoverOnForm->LocalX = to_string((int)pos.x);
	mHoverOnForm->LocalY = to_string((int)pos.y);
	mHoverOnForm->Width = to_string((int)hoveredView->GetWidth());
	mHoverOnForm->Height = to_string((int)hoveredView->GetHeight());
	parentView->AddChild(mHoverOnForm);
	mHoverOnForm->LoadAll();
}

void ObjUI::Reset()
{
	mActiveOnItem->Detach();
	mHoverOnForm->Detach();

	int gridCount = mGridView->ColumnCount * mGridView->RowCount;
	for (int i = 0; i < gridCount; ++i)
		mGridView->GetChild(i)->ClearChilds();

	for (auto slot : mSlotViews)
		slot.second->ClearChilds();
}

bool ObjUI::IsFull()
{
	return mGridView->FindEmptyChild() == nullptr;
}

bool ObjUI::PickItem(const DBItem& itemDB)
{
	if (IsFull())
		return false;

	jView* emptyView = mGridView->FindEmptyChild();
	DBItem newItem = itemDB;
	newItem.owner = mPlayer->GetID();
	newItem.state = ItemState::Carraied;
	newItem.posIndex = emptyView->UserData[FN_CellIndex].val<int>();
	newItem.Save();

	jView* newItemView = CreateItemView(newItem.GetID());
	emptyView->AddChild(newItemView);
	newItemView->LoadAll();
	return true;
}

void jEventUI::OnMouseClick(Vector2n pt, int type)
{
	ObjUI* obj = (ObjUI*)GetGameObject();
	jView* view = mUIEngine->FindTopView(pt.x, pt.y);
	if (view == nullptr && obj->mSelectedItem != nullptr)
	{
		u32 itemID = obj->mSelectedItem->UserData[FN_GameObject].val<u32>();
		if (obj->mSelectedItem->GetParent()->GetParent() != obj->mGridView)
			obj->UnEquip(itemID, 0);
		
		obj->DropItem(itemID);

		obj->mActiveOnItem->Detach();
		obj->mSelectedItem->Detach();
		delete obj->mSelectedItem;
		obj->mSelectedItem = nullptr;
	}
}
