#include "jLog.h"
#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jInput.h"
#include "jSound.h"

int gScreenWidth = 640;
int gScreenHeight = 480;
HINSTANCE g_hInst = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int main()
{
	HWND handle;
	WNDCLASSEX wc;

	g_hInst = GetModuleHandle(NULL);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInst;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = "windowTitle";

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "asdfasdf", "Error", MB_OK);
		return 0;
	}

	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZE;
	handle = CreateWindowEx(WS_EX_APPWINDOW, "windowTitle", "windowTitle", nStyle, 0, 0, gScreenWidth, gScreenHeight, nullptr, nullptr, g_hInst, nullptr);
	if (handle == nullptr)
	{
		return 0;
	}

	jSound::GetInst().Initialize(handle);
	jSound::GetInst().LoadWaveFile("sound01.wav");

	jInput::GetInst().Initialize(g_hInst, handle, gScreenWidth, gScreenHeight);
	jRenderer::GetInst().Initialize(handle, gScreenWidth, gScreenHeight, false, true);
	jGameObjectMgr::GetInst().Initialize();

	ShowWindow(handle, SW_SHOW);
	SetForegroundWindow(handle);
	SetFocus(handle);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			jInput::GetInst().Update();
			jGameObjectMgr::GetInst().RunObjects();
		}
	}

	jGameObjectMgr::GetInst().Release();
	jRenderer::GetInst().Release();
	jInput::GetInst().Release();

	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
	}break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		DestroyWindow(hWnd);
	}break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
