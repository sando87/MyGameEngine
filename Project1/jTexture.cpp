#include "jTexture.h"
#include "jRenderer.h"
#include "jUtils.h"
#include "jLog.h"
#include "jGlobalStruct.h"

jTexture::jTexture()
{
}


jTexture::~jTexture()
{
}

bool jTexture::Initialize(string _filename)
{
	int width = 0;
	int height = 0;
	chars buf = jUtils::LoadTarga(_filename, height, width);

	// targa 이미지 데이터를 메모리에 로드합니다.
	if (!buf)
	{
		_warn();
		return false;
	}

	//텍스처의 구조체를 설정합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스처를 생성합니다.
	ID3D11Texture2D* texture = nullptr;
	HRESULT hResult = jRenderer::GetInst().GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	//  targa 이미지 데이터의 너비 사이즈를 설정합니다.
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// targa 이미지 데이터를 텍스처에 복사합니다.
	jRenderer::GetInst().GetDeviceContext()->UpdateSubresource(texture, 0, NULL, &buf[0], rowPitch, 0);

	// 셰이더 리소스 뷰 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처의 셰이더 리소스 뷰를 만듭니다.
	hResult = jRenderer::GetInst().GetDevice()->CreateShaderResourceView(texture, &srvDesc, &mTextureView);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	// 이 텍스처에 대해 밉맵을 생성합니다.
	jRenderer::GetInst().GetDeviceContext()->GenerateMips(mTextureView);

	// 이미지 데이터가 텍스처에 로드 되었으므로 targa 이미지 데이터를 해제합니다.

	texture->Release();
	texture = nullptr;

	return true;
}

void jTexture::Release()
{
	//텍스처 뷰 리소스를 해제한다.
	if (mTextureView)
	{
		mTextureView->Release();
		mTextureView = nullptr;
	}
}

bool jTexture::Load_FromRes(MyResBase * _res)
{
	if (_res == NULL)
		return false;

	MyRes_CreateTexture *pRes = (MyRes_CreateTexture *)_res;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc = pRes->desc;

	//textureDesc.Height = height;
	//textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	ID3D11Texture2D* texture = nullptr;
	HRESULT hResult = jRenderer::GetInst().GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	jRenderer::GetInst().GetDeviceContext()->UpdateSubresource(texture, 0, NULL, pRes->data, pRes->head.reserve1, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = pRes->desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hResult = jRenderer::GetInst().GetDevice()->CreateShaderResourceView(texture, &srvDesc, &mTextureView);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	// 이 텍스처에 대해 밉맵을 생성합니다.
	jRenderer::GetInst().GetDeviceContext()->GenerateMips(mTextureView);

	texture->Release();
	texture = nullptr;

	return true;
}
