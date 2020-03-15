#include "ObjUI.h"
#include "jInputEvent.h"
#include "jShaderUI.h"
#include "jImage.h"
#include "jInventory.h"
#include "ObjItem.h"

#include <jViewGrid.h>
#include <jViewImage.h>


#pragma comment(lib,"jGUI.lib")

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
	

	mViewGrid = (jViewGrid*)mUIEngine->FindView("InvenGrid");
	mViewGrid->UserData[FN_Category] = "null";
	mViewSlots[0] = mUIEngine->FindView("SlotA");
	mViewSlots[0]->UserData[FN_Category] = "armor";
	mViewSlots[1] = mUIEngine->FindView("SlotB");
	mViewSlots[1]->UserData[FN_Category] = "gloves";
	mCellWidth = mViewGrid->GetWidth() / mViewGrid->ColumnCount;
	mCellHeight = mViewGrid->GetHeight() / mViewGrid->RowCount;


	mViewGrid->EventGridEnter = [&](jView* view, Point2 pt){ DoHightlight(view);};
	mViewGrid->EventGridLeave = [&](jView* view, Point2 pt) { mViewLight->Detach(); };
	mViewGrid->EventGridClick = [&](jView* view, Point2 pt) { DoClickEvent(view); };
	mViewSlots[0]->EventMouseEnter = [&](jView* view, Point2 pt) { DoHightlight(view); };
	mViewSlots[0]->EventMouseLeave = [&](jView* view, Point2 pt) { mViewLight->Detach(); };
	mViewSlots[0]->EventMouseClick = [&](jView* view, Point2 pt) { DoClickEvent(view); };
	mViewSlots[1]->EventMouseEnter = [&](jView* view, Point2 pt) { DoHightlight(view); };
	mViewSlots[1]->EventMouseLeave = [&](jView* view, Point2 pt) { mViewLight->Detach(); };
	mViewSlots[1]->EventMouseClick = [&](jView* view, Point2 pt) { DoClickEvent(view); };


	mViewActive = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	mViewActive->Width = "1.0";
	mViewActive->Height = "1.0";
	mViewActive->Image.filename = "itemgrade2.png";
	mViewActive->Image.top = 0;
	mViewActive->Image.bottom = 1.0f;
	mViewActive->Image.left = 0.125f * 5;
	mViewActive->Image.right = 0.125f * 6;
	mViewActive->Enable = false;

	mViewLight = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	mViewLight->Width = "1.0";
	mViewLight->Height = "1.0";
	mViewLight->Image.filename = "itemgrade2.png";
	mViewLight->Image.top = 0;
	mViewLight->Image.bottom = 1.0f;
	mViewLight->Image.left = 0.125f * 5;
	mViewLight->Image.right = 0.125f * 6;
	mViewLight->Enable = false;

}

void ObjUI::OnUpdate()
{
	mUIEngine->MouseEventCall();
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

void ObjUI::DoClickEvent(jView* clickedView)
{
	bool clickedGrid = (clickedView->GetParent() == mViewGrid);
	jView* itemView = clickedView->Childs.empty() ? nullptr : clickedView->Childs.front();
	if (itemView == nullptr && mSelectedView == nullptr)
		return;

	if (clickedGrid)
	{
		if (mSelectedView != nullptr && itemView != nullptr && mSelectedView->GetParent()->GetParent() != mViewGrid)
		{
			string cateA = mSelectedView->UserData[FN_Category].val<string>();
			string cateB = itemView->UserData[FN_Category].val<string>();
			if (cateA != cateB)
				return;
		}
	}
	else
	{
		if (mSelectedView != nullptr)
		{
			string cateA = mSelectedView->UserData[FN_Category].val<string>();
			string cateB = clickedView->UserData[FN_Category].val<string>();
			if (cateA != cateB)
				return;
		}
	}

	if (itemView == nullptr)
	{
		//MoveView
		mSelectedView->Detach();
		clickedView->AddChild(mSelectedView);
		mSelectedView->LoadAll();

		mViewActive->Detach();
		mSelectedView = nullptr;

	}
	else if (mSelectedView == nullptr)
	{
		//SelectView
		mSelectedView = itemView;
		mSelectedView->AddChild(mViewActive);
		mViewActive->LoadAll();
	}
	else if (mSelectedView == itemView)
	{
		//UnSelectView
		mViewActive->Detach();
		mSelectedView = nullptr;
	}
	else
	{
		//SwapView
		jView* backView = mSelectedView->GetParent();
		mSelectedView->Detach();
		itemView->Detach();
		backView->AddChild(itemView);
		clickedView->AddChild(mSelectedView);
		mSelectedView->LoadAll();
		itemView->LoadAll();

		mViewActive->Detach();
		mSelectedView = nullptr;
	}
}

void ObjUI::DoHightlight(jView * hoveredView)
{
	jView* parentView = hoveredView->GetParent();
	Point2 pos = hoveredView->GetLocalPos();
	mViewLight->LocalX = to_string((int)pos.x);
	mViewLight->LocalY = to_string((int)pos.y);
	mViewLight->Width = to_string((int)hoveredView->GetWidth());
	mViewLight->Height = to_string((int)hoveredView->GetHeight());
	parentView->AddChild(mViewLight);
	mViewLight->LoadAll();
}

void ObjUI::Reset()
{
	mViewActive->Detach();
	mViewLight->Detach();

	int gridCount = mViewGrid->ColumnCount * mViewGrid->RowCount;
	for (int i = 0; i < gridCount; ++i)
		mViewGrid->GetChild(i)->ClearChilds();

	int cnt = sizeof(mViewSlots) / sizeof(mViewSlots[0]);
	for (int i = 0; i < cnt; ++i)
	{
		if (mViewSlots[i] != nullptr)
			mViewSlots[i]->ClearChilds();
	}
}

bool ObjUI::AddItem(ObjItem * item)
{
	const DBItem& itemInfo = item->GetDBItem();
	const DBItemResource& itemResInfo = item->GetDBItemResource();
	int posIdx = itemInfo.posIndex;
	jView* emptyView = mViewGrid->GetChild(posIdx);
	if (!emptyView->Childs.empty())
	{
		emptyView = mViewGrid->FindEmptyChild();
		if (emptyView == nullptr) //inventory full
			return false;

		posIdx = emptyView->UserData["Idx"].val<int>();
		if (EventMoveItem != nullptr)
			EventMoveItem(item, posIdx);
	}

	jViewImage* viewbg = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	viewbg->Width = "1.0";
	viewbg->Height = "1.0";
	viewbg->Image.filename = "itemgrade2.png";
	viewbg->Image.top = 0;
	viewbg->Image.bottom = 1;
	viewbg->Image.left = 0.125 * itemInfo.grade;
	viewbg->Image.right = 0.125 * (itemInfo.grade + 1);
	viewbg->Enable = false;
	viewbg->UserData[FN_GameObject] = item;
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
	viewfg->UserData[FN_GameObject] = item;
	viewfg->UserData[FN_Category] = itemResInfo.category;

	viewbg->AddChild(viewfg);
	emptyView->AddChild(viewbg);
	viewbg->LoadAll();
	return true;
}
