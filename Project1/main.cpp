#include "jLog.h"
#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jInput.h"
#include "jSound.h"
#include "jOS_APIs.h"

int gScreenWidth = 640;
int gScreenHeight = 480;
HINSTANCE g_hInst = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


class A {
	std::string s;
	std::weak_ptr<A> other;

public:
	A(const std::string& s) : s(s) { std::cout << "자원을 획득함!" << std::endl; }

	~A() { std::cout << "소멸자 호출!" << std::endl; }

	void set_other(std::weak_ptr<A> o) { other = o; }
	void access_other() {
		std::shared_ptr<A> o = other.lock();
		if (o) {
			std::cout << "접근 : " << o->name() << std::endl;
		}
		else {
			std::cout << "이미 소멸됨 ㅠ" << std::endl;
		}
	}
	std::string name() { return s; }
};

int main()
{
	//std::vector<std::shared_ptr<A>> vec;
	//vec.push_back(std::make_shared<A>("자원 1"));
	//vec.push_back(std::make_shared<A>("자원 2"));
	//
	//vec[0]->set_other(vec[1]);
	//vec[1]->set_other(vec[0]);
	//
	//// pa 와 pb 의 ref count 는 그대로다.
	//std::cout << "vec[0] ref count : " << vec[0].use_count() << std::endl;
	//std::cout << "vec[1] ref count : " << vec[1].use_count() << std::endl;
	//
	//// weak_ptr 로 해당 객체 접근하기
	//vec[0]->access_other();
	//
	//// 벡터 마지막 원소 제거 (vec[1] 소멸)
	//vec.pop_back();
	//vec[0]->access_other();  // 접근 실패!


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

	int nStyle = WS_POPUP; //WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZE;
	handle = CreateWindowEx(WS_EX_APPWINDOW, "windowTitle", "windowTitle", nStyle, 0, 0, gScreenWidth, gScreenHeight, nullptr, nullptr, g_hInst, nullptr);
	if (handle == nullptr)
	{
		return 0;
	}

	jOS_APIs::SetHandle(handle);
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
