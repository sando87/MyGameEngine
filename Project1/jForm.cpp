#include "jForm.h"
#include "jShaderUI.h"
#include "jImage.h"
#include <jUISystem.h>


jForm::jForm(jUISystem* form)
{
	mForm = form;
}


jForm::~jForm()
{
}

void jForm::OnLoad()
{
	mShader = GetGameObject()->FindComponent<jShaderUI>();

	mForm->SetResourcePath("./res/ui/");
	mForm->EventDrawFill = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, nullptr);
	};
	mForm->EventDrawOutline = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, nullptr);
	};
	mForm->EventDrawTexture = [&](DrawingParams params) {
		VertexFormatPTC vert[4];
		ConvertDrawParam(params, vert);
		mShader->DrawRect(vert, params.texture);
	};
	mForm->OpLoadTexture = [&](jUIBitmap* bitmap) {
		void* ptr = nullptr;
		if (bitmap->fullname.length() > 0)
		{
			jImage img(bitmap->fullname);
			img.LoadImgFile();
			chars buf = img.GetBuffer();
			bitmap->width = img.GetWidth();
			bitmap->height = img.GetHeight();
			bitmap->byteperpixel = 4;
			ptr = mShader->LoadTextureRes((unsigned char*)&buf[0], img.GetWidth(), img.GetHeight());

		}
		else if (!bitmap->buf.empty())
		{
			ptr = mShader->LoadTextureRes((unsigned char*)&bitmap->buf[0], bitmap->width, bitmap->height);
		}

		return ptr;
	};
	mForm->OpReleaseTexture = [&](void* ptr) {
		mShader->ReleaseTextureRes(ptr);
	};
	mForm->ParseJson(GetJsonFilename());
	mForm->LoadViews();
}

void jForm::OnUpdate()
{
	mForm->MouseEventCall();
}

void jForm::ConvertDrawParam(DrawingParams& params, VertexFormatPTC vert[4])
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