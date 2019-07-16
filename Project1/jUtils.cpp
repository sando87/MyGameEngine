#include "jUtils.h"
#include "jMath.h"
#include "jLog.h"
#include <sstream>
#include <iostream>
#include <Windows.h>
#include <stdio.h>

jUtils::jUtils()
{
}


jUtils::~jUtils()
{
}

void jUtils::GetPerspectiveFovLH(Matrix4& _mat, double _fovDeg, double _aspect, double _near, double _far)
{
	_errorif(_near == _far || _aspect == 0);
	
	_mat.identity();
	_mat[5] = 1 / tan(DegToRad(_fovDeg * 0.5));
	_mat[0] = _mat[5] / _aspect;
	_mat[10] = _far / (_far - _near);
	_mat[11] = 1.0;
	_mat[14] = (_far*_near) / (_near - _far);
	_mat[15] = 0.0;
}
void jUtils::Split(string _str, const char* _del, vector<string>& _vec)
{
	trim(_str);
	std::stringstream stringStream(_str);
	std::string line;
	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(_del, prev)) != std::string::npos)
		{
			if (pos > prev)
				_vec.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			_vec.push_back(line.substr(prev, std::string::npos));
	}
}


bool jUtils::LoadTarga(string filename, int& height, int& width, int& _bufSize, unsigned char*& _buf, bool _isInvert)
{
	// targa 파일을 바이너리 모드로 파일을 엽니다.
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽어옵니다.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일헤더에서 중요 정보를 얻어옵니다.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// 파일이 32bit 인지 24bit인지 체크합니다.
	if (bpp != 32)
	{
		return false;
	}

	// 32 비트 이미지 데이터의 크기를 계산합니다.
	int imageSize = width * height * 4;

	//  targa 이미지 데이터 용 메모리를 할당합니다.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa 이미지 데이터를 읽습니다.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa 대상 데이터에 대한 메모리를 할당합니다.
	_buf = new unsigned char[imageSize];
	if (!_buf)
	{
		return false;
	}

	_bufSize = imageSize;
	// targa 대상 데이터 배열에 인덱스를 초기화합니다.
	int index = 0;

	// targa 이미지 데이터에 인덱스를 초기화합니다.
	int k = _isInvert ? ((width * height * 4) - (width * 4)) : 0;

	// 이제 targa 형식이 거꾸로 저장되었으므로 올바른 순서로 targa 이미지 데이터를 targa 대상 배열에 복사합니다.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			_buf[index + 0] = targaImage[k + 2];  // 빨강
			_buf[index + 1] = targaImage[k + 1];  // 녹색
			_buf[index + 2] = targaImage[k + 0];  // 파랑
			_buf[index + 3] = targaImage[k + 3];  // 알파

														 // 인덱스를 targa 데이터로 증가시킵니다.
			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작 부분에서 이전 행으로 다시 설정합니다.
		if(_isInvert)
			k -= (width * 8);
	}

	// 대상 배열에 복사 된 targa 이미지 데이터를 해제합니다.
	delete[] targaImage;
	targaImage = nullptr;

	return true;
}

bool jUtils::LoadFile(string path, int* _bufSize, char** _buf)
{
	FILE* filePtr = NULL;
	int filesize = 0;
	if (fopen_s(&filePtr, path.c_str(), "rb") != 0)
		return false;

	fseek(filePtr, 0, SEEK_END);
	filesize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	char* pBuf = (char*)malloc(filesize);
	fread(pBuf, filesize, 1, filePtr);

	*_bufSize = filesize;
	*_buf = pBuf;

	fclose(filePtr);
	return true;
}
//example path is like "D:\\temp\\*.*";
void jUtils::ForEachFiles2(void* _object, const char* _path, function<bool(void*, string)> _func)
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
string jUtils::FindFile(string _path, string _filter)
{
	string ret = "";
	jUtils::ForEachFiles2(nullptr, (_path + _filter).c_str(), [&](void* _obj, string _str) {
		ret = _str;
		return STOP;
	});
	return ret;
}
bool jUtils::MyCopyFile(string _from, string _to)
{
	return CopyFile(_from.c_str(), _to.c_str(), true);
}
void jUtils::SaveToFile(string path, string filename, string data, bool isAttach)
{
	char name[260] = { 0, };
	sprintf_s(name, "%s\\%s", path.c_str(), filename.c_str());

	FILE *pFile = NULL;

	if (!isAttach)
	{
		fopen_s(&pFile, name, "wb");
		if (pFile == NULL)
			return;
	}
	else
	{
		fopen_s(&pFile, name, "ab");
		if (pFile == NULL)
			return;
	}

	int size = data.size();
	fwrite(data.c_str(), 1, size, pFile);

	fclose(pFile);
	return;
}
void jUtils::ForEachFiles(void* _object, const char* _path, bool(*_func)(void *_this, char *_filename))
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