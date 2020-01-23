#pragma once

#include "jDirectXheader.h"
#include "junks.h"
#include "jComponent.h"
#include "jShaderHeader.h"
#include "jGameObjectMgr.h"
#include "jGraphicResources.h"

#define ResNameExt_Bolb ".bolb"
#define ResName_SamplerState_Default "samplerState.default"
#define VS_FUNCNAME "jVS"
#define PS_FUNCNAME "jPS"

class jMesh;
class jImage;

class jShader : public jComponent
{
public:
	jShader();
	virtual ~jShader() {}

	virtual void OnLoad() {}
	virtual void OnUpdate() {}
	virtual bool OnRender() = 0;

protected:
	ID3D11VertexShader *			CacheVertexShader(string keyName);
	ID3D11PixelShader *				CachePixelShader(string keyName);
	ID3D11SamplerState *			CacheSamplerState(string keyName);
	ID3D11ShaderResourceView *	CacheTextureView(string keyName);
	ID3D11Buffer *						CacheIndexedBuffer(string keyName);


	ID3D11ShaderResourceView*			LoadDumpTexture(string name);
	ID3D10Blob *								CompileShader(string filename);
	ID3D11InputLayout *						CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layouts, int count, string vsFileName);
	ID3D11Buffer *								CreateConstBuffer(D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* subData);
	ID3D11SamplerState *					CreateSamplerState(D3D11_SAMPLER_DESC* desc);
	ID3D11ShaderResourceView *			CreateTextureView(ID3D11Texture2D* texture, D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc);
	ID3D11Texture2D *						CreateTexture2D(D3D11_TEXTURE2D_DESC* texDesc, 
														D3D11_SUBRESOURCE_DATA* texSubData, 
														int width, int height, char* buf);

	ID3D11Device*				mDev;
	ID3D11DeviceContext*	mDevContext;
	jGraphicResources*		mGraphicResources;

	ID3D11VertexShader *	mVertexShader;
	ID3D11PixelShader *		mPixelShader;

	jMesh *						mBasicMesh;
	jImage *						mBasicImage;

	Property_GetSetter(bool, AlphaOn, false)
	Property_GetSetter(bool, DepthOn, true)
	Property_GetSetter(float, RenderOrder, 0)
};

