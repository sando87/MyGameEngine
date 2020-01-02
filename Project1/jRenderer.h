#ifndef __JRENDERER_H__
#define __JRENDERER_H__

#include "jDirectXheader.h"
#include "junks.h"

#define SCREEN_NEAR 1.0f
#define SCREEN_FAR 1000.0f

class jRenderer
{
public:
	static jRenderer& GetInst() { static jRenderer renderer; return renderer; }

private:
	jRenderer();
	~jRenderer();

public:
	bool Initialize(HWND _hWnd, int _width, int _height, bool _isFullScreen, bool _isVsync);
	bool InitRenderStates();
	void Release();
	bool Render();
	void BeginScene();
	void EndScene();

	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;



	Property_Getter(ID3D11BlendState *, BS_AlphaOff)
	Property_Getter(ID3D11BlendState *, BS_AlphaOn)
	Property_Getter(ID3D11DepthStencilState *, DSS_DepthOff)
	Property_Getter(ID3D11DepthStencilState *, DSS_DepthOn)
	
};

#endif