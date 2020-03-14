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
	mViewGrid->UserData[FN_Category] = 0;
	mViewSlots[0] = mUIEngine->FindView("SlotA");
	mViewSlots[0]->UserData[FN_Category] = 1;
	mViewSlots[1] = mUIEngine->FindView("SlotB");
	mViewSlots[1]->UserData[FN_Category] = 2;
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
	mViewActive->Image.filename = "itemgrade.png";
	mViewActive->Image.top = 0;
	mViewActive->Image.bottom = 1.0f;
	mViewActive->Image.left = 0.9f;
	mViewActive->Image.right = 1.0f;
	mViewActive->Enable = false;

	mViewLight = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	mViewLight->Width = "1.0";
	mViewLight->Height = "1.0";
	mViewLight->Image.filename = "itemgrade.png";
	mViewLight->Image.top = 0;
	mViewLight->Image.bottom = 1.0f;
	mViewLight->Image.left = 0.9f;
	mViewLight->Image.right = 1.0f;
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
			int cateA = mSelectedView->UserData[FN_Category].val<int>();
			int cateB = itemView->UserData[FN_Category].val<int>();
			if (cateA != cateB)
				return;
		}
	}
	else
	{
		if (mSelectedView != nullptr)
		{
			int cateA = mSelectedView->UserData[FN_Category].val<int>();
			int cateB = clickedView->UserData[FN_Category].val<int>();
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
	const ItemProperty& prop = item->GetProperty();
	int posIdx = prop.posIndex;
	jView* emptyView = mViewGrid->GetChild(posIdx);
	if (!emptyView->Childs.empty())
	{
		emptyView = mViewGrid->FindEmptyChild();
		if (emptyView == nullptr) //inventory full
			return false;

		posIdx = emptyView->UserData["Idx"].val<int>();
		if (EventItemMoved != nullptr)
			EventItemMoved(item, posIdx);
	}

	jViewImage* viewbg = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	viewbg->Width = "1.0";
	viewbg->Height = "1.0";
	viewbg->Image.filename = "itemgrade.png";
	viewbg->Image.top = 0;
	viewbg->Image.bottom = 1;
	viewbg->Image.left = 0.1 * prop.grade;
	viewbg->Image.right = 0.1 * (prop.grade + 1);
	viewbg->Enable = false;
	viewbg->UserData[FN_GameObject] = item;
	viewbg->UserData[FN_Category] = (int)prop.category;

	jViewImage* viewfg = (jViewImage*)mUIEngine->CreateView(jViewType::Image);
	viewfg->Width = "1.0";
	viewfg->Height = "1.0";
	viewfg->Image.filename = prop.UIImageFilename;
	viewfg->Image.top = prop.UVtop;
	viewfg->Image.bottom = prop.UVbottom;
	viewfg->Image.left = prop.UVleft;
	viewfg->Image.right = prop.UVright;
	viewfg->Enable = false;
	viewfg->UserData[FN_GameObject] = item;
	viewfg->UserData[FN_Category] = (int)prop.category;

	viewbg->AddChild(viewfg);
	emptyView->AddChild(viewbg);
	viewbg->LoadAll();
	return true;
}
