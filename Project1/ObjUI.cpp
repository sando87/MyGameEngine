#include "ObjUI.h"
#include "jInputEvent.h"
#include "jShaderUI.h"

#pragma comment(lib,"jGUI.lib")

class jShaderUIEngine : public jShaderUI
{
private:
	jUISystem * mUIEngine;
	virtual void OnLoad() 
	{ 
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
		void* ptr;
		if (bitmap->fullname.length() > 0 && bitmap->buf.empty())
		{
			int width = 0;
			int height = 0;
			chars buf = jUtils::LoadTarga(bitmap->fullname, height, width);
			ptr = mShader->LoadTextureRes((unsigned char*)&buf[0], width, height);

		}
		else
		{
			ptr = mShader->LoadTextureRes((unsigned char*)&bitmap->buf[0], bitmap->width, bitmap->height);
		}
		
		return ptr;
	};
	mUIEngine->OpReleaseTexture = [&](void* ptr) {
		mShader->ReleaseTextureRes(ptr);
	};

	mUIEngine->ParseJson("test.json");
	mUIEngine->LoadViews();
	
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
