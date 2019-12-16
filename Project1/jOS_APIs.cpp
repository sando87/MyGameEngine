#include "jOS_APIs.h"
#include <Windows.h>

HWND g_hwnd = nullptr;

jOS_APIs::jOS_APIs()
{
}


jOS_APIs::~jOS_APIs()
{
}

void jOS_APIs::SetHandle(void *hwnd)
{
	g_hwnd = (HWND)hwnd;
}
void jOS_APIs::ForEachFiles(void* _object, const char* _path, function<bool(void*, string)> _func)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;
	hFind = FindFirstFile(_path, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (strncmp(data.cFileName, ".", 260) == 0)
				continue;
			if (strncmp(data.cFileName, "..", 260) == 0)
				continue;

			if (!_func(_object, data.cFileName))
				break;

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

void jOS_APIs::MyCopyFile(string _from, string _to)
{
	CopyFile(_from.c_str(), _to.c_str(), true);
}
Vector2 jOS_APIs::GetCursorScreenPos()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hwnd, &pt);
	return Vector2(pt.x, pt.y);
}