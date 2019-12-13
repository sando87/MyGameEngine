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

	// targa �̹��� �����͸� �޸𸮿� �ε��մϴ�.
	if (!buf)
	{
		_warn();
		return false;
	}

	//�ؽ�ó�� ����ü�� �����մϴ�.
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

	// �� �ؽ�ó�� �����մϴ�.
	ID3D11Texture2D* texture = nullptr;
	HRESULT hResult = jRenderer::GetInst().GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	//  targa �̹��� �������� �ʺ� ����� �����մϴ�.
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// targa �̹��� �����͸� �ؽ�ó�� �����մϴ�.
	jRenderer::GetInst().GetDeviceContext()->UpdateSubresource(texture, 0, NULL, &buf[0], rowPitch, 0);

	// ���̴� ���ҽ� �� ����ü�� �����մϴ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ�ó�� ���̴� ���ҽ� �並 ����ϴ�.
	hResult = jRenderer::GetInst().GetDevice()->CreateShaderResourceView(texture, &srvDesc, &mTextureView);
	if (FAILED(hResult))
	{
		_warn();
		return false;
	}

	// �� �ؽ�ó�� ���� �Ӹ��� �����մϴ�.
	jRenderer::GetInst().GetDeviceContext()->GenerateMips(mTextureView);

	// �̹��� �����Ͱ� �ؽ�ó�� �ε� �Ǿ����Ƿ� targa �̹��� �����͸� �����մϴ�.

	texture->Release();
	texture = nullptr;

	return true;
}

void jTexture::Release()
{
	//�ؽ�ó �� ���ҽ��� �����Ѵ�.
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

	// �� �ؽ�ó�� ���� �Ӹ��� �����մϴ�.
	jRenderer::GetInst().GetDeviceContext()->GenerateMips(mTextureView);

	texture->Release();
	texture = nullptr;

	return true;
}
