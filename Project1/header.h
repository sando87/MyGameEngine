#pragma once


#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

#pragma warning(disable: 4200)

#define PATH_RESOURCE "D:\\temp\\"


#define DIRECTINPUT_VERSION 0x0800
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dsound.h>
#include <D3Dcompiler.h>
#include <directxmath.h>
#include <dinput.h>
//#include <D3DX10math.h>
using namespace DirectX;