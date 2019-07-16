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
	// targa ������ ���̳ʸ� ���� ������ ���ϴ�.
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		return false;
	}

	// ���� ����� �о�ɴϴ�.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ����������� �߿� ������ ���ɴϴ�.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// ������ 32bit ���� 24bit���� üũ�մϴ�.
	if (bpp != 32)
	{
		return false;
	}

	// 32 ��Ʈ �̹��� �������� ũ�⸦ ����մϴ�.
	int imageSize = width * height * 4;

	//  targa �̹��� ������ �� �޸𸮸� �Ҵ��մϴ�.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa �̹��� �����͸� �н��ϴ�.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// ������ �ݽ��ϴ�.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa ��� �����Ϳ� ���� �޸𸮸� �Ҵ��մϴ�.
	_buf = new unsigned char[imageSize];
	if (!_buf)
	{
		return false;
	}

	_bufSize = imageSize;
	// targa ��� ������ �迭�� �ε����� �ʱ�ȭ�մϴ�.
	int index = 0;

	// targa �̹��� �����Ϳ� �ε����� �ʱ�ȭ�մϴ�.
	int k = _isInvert ? ((width * height * 4) - (width * 4)) : 0;

	// ���� targa ������ �Ųٷ� ����Ǿ����Ƿ� �ùٸ� ������ targa �̹��� �����͸� targa ��� �迭�� �����մϴ�.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			_buf[index + 0] = targaImage[k + 2];  // ����
			_buf[index + 1] = targaImage[k + 1];  // ���
			_buf[index + 2] = targaImage[k + 0];  // �Ķ�
			_buf[index + 3] = targaImage[k + 3];  // ����

														 // �ε����� targa �����ͷ� ������ŵ�ϴ�.
			k += 4;
			index += 4;
		}

		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���� �κп��� ���� ������ �ٽ� �����մϴ�.
		if(_isInvert)
			k -= (width * 8);
	}

	// ��� �迭�� ���� �� targa �̹��� �����͸� �����մϴ�.
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