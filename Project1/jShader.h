#pragma once

#include "jDirectXheader.h"
#include "junks.h"
#include "jComponent.h"
#include "jShaderHeader.h"
#include "jGameObjectMgr.h"
#include "jGraphicResources.h"


class ObjCamera;

class jShader : public jComponent
{
public:
	jShader();
	virtual ~jShader() {}

	virtual void OnLoad() {}
	virtual void OnUpdate() {}
	virtual bool OnRender(ObjCamera* cam) = 0;

	ShaderBufferBasic&	GetParamBasic() { return mParamBasic; }
	ShaderBufferSkin&	GetParamSkin() { return mParamSkin; }

	void* LoadTextureRes(unsigned char* buf, int width, int height);
	void ReleaseTextureRes(void* ptr);

protected:
	ID3D10Blob *					CompileShader(string filename);
	ID3D11InputLayout *				CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layouts, int count, string vsFileName);
	ID3D11Buffer *					CreateConstBuffer(D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* subData);
	ID3D11SamplerState *			CreateSamplerState(D3D11_SAMPLER_DESC* desc);
	ID3D11ShaderResourceView *		CreateTextureView(ID3D11Texture2D* texture, D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc);
	ID3D11Texture2D *				CreateTexture2D(D3D11_TEXTURE2D_DESC* texDesc, 
														D3D11_SUBRESOURCE_DATA* texSubData, 
														int width, int height, unsigned char* buf);


	void CacheCBufferBasic();
	void CacheCBufferSkin();
	void CacheSamplerState();
	void CacheBlendState();
	void CacheDepthState();
	void CacheVS(string keyFullname);
	void CachePS(string keyFullname);
	void CacheInputLayout(D3D11_INPUT_ELEMENT_DESC * layouts, int count, string keyLayout, string keyVS);
	void CacheCBVertex(void* buf, int size, string key);
	void CacheCBIndex(void* buf, int size, string key);
	void AddCachedTextureView(void* buf, int width, int height, bool isCompressed, string key);

	void LoadCompentIndicies();
	void LoadCompentTextures();

	void SetRenderContext(ObjCamera * cam);
	void Draw(u32 stride, D3D_PRIMITIVE_TOPOLOGY primitive, u32 vertCount, u32 indiCount);


	ID3D11Device*			mDev;
	ID3D11DeviceContext*	mDevContext;
	jGraphicResources*		mGraphicResources;

	ID3D11VertexShader *	mVertexShader;
	ID3D11PixelShader *		mPixelShader;
	ID3D11Buffer *			mCBBasic;
	ID3D11Buffer *			mCBSkin;
	ID3D11SamplerState *	mSamplerState;
	ID3D11InputLayout *		mInputLayout;
	ID3D11Buffer *			mCBVertex;
	ID3D11Buffer *			mCBIndex;
	ID3D11BlendState *			mBlendStateOff;
	ID3D11BlendState *			mBlendStateOn;
	ID3D11DepthStencilState *	mDepthStencilStateOff;
	ID3D11DepthStencilState *	mDepthStencilStateOn;
	vector<ID3D11ShaderResourceView *>	mTextureViews;

	ShaderBufferBasic	mParamBasic;
	ShaderBufferSkin	mParamSkin;


	Property_GetSetter(bool, AlphaOn, false)
	Property_GetSetter(bool, DepthOn, true)
	Property_GetSetter(float, RenderOrder, 0)
};

