
#include "jUtils.h"
#include "junks.h"
#include "jOS_APIs.h"

jUtils::jUtils()
{
}


jUtils::~jUtils()
{
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

strings jUtils::Split2(string _str, const char* _del)
{
	strings rets;
	trim(_str);
	std::stringstream stringStream(_str);
	std::string line;
	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(_del, prev)) != std::string::npos)
		{
			if (pos > prev)
				rets->push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			rets->push_back(line.substr(prev, std::string::npos));
	}
	return rets;
}

chars jUtils::LoadTarga(string filename, int& height, int& width, bool _isInvert)
{
	// targa 파일을 바이너리 모드로 파일을 엽니다.
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		_printlog("fail to load %s\n", filename);
		return chars();
	}

	// 파일 헤더를 읽어옵니다.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		_printlog("fail to load .tga file\n");
		return chars();
	}

	// 파일헤더에서 중요 정보를 얻어옵니다.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	// 파일이 32bit 인지 24bit인지 체크합니다.
	if (bpp != 32)
	{
		_printlog("tga file error (only 32bit possible)\n");
		return chars();
	}

	// 32 비트 이미지 데이터의 크기를 계산합니다.
	int imageSize = width * height * 4;

	chars targaRawData;
	targaRawData->resize(imageSize);
	fread(&targaRawData[0], imageSize, 1, filePtr);

	// 파일을 닫습니다.
	fclose(filePtr);


	chars pBuf;
	pBuf->resize(imageSize);
	// targa 대상 데이터 배열에 인덱스를 초기화합니다.
	int index = 0;

	// targa 이미지 데이터에 인덱스를 초기화합니다.
	int k = _isInvert ? ((width * height * 4) - (width * 4)) : 0;

	// 이제 targa 형식이 거꾸로 저장되었으므로 올바른 순서로 targa 이미지 데이터를 targa 대상 배열에 복사합니다.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i<width; i++)
		{
			pBuf[index + 0] = targaRawData[k + 2];  // 빨강
			pBuf[index + 1] = targaRawData[k + 1];  // 녹색
			pBuf[index + 2] = targaRawData[k + 0];  // 파랑
			pBuf[index + 3] = targaRawData[k + 3];  // 알파

														 // 인덱스를 targa 데이터로 증가시킵니다.
			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작 부분에서 이전 행으로 다시 설정합니다.
		if(_isInvert)
			k -= (width * 8);
	}

	return pBuf;
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
chars jUtils::LoadFile2(string path, int& _bufSize)
{
	FILE* filePtr = NULL;
	int filesize = 0;
	if (fopen_s(&filePtr, path.c_str(), "rb") != 0)
		return chars();

	fseek(filePtr, 0, SEEK_END);
	filesize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	chars pBuf;
	pBuf->resize(filesize);
	fread(&pBuf[0], filesize, 1, filePtr);

	_bufSize = filesize;

	fclose(filePtr);
	return pBuf;
}
strings jUtils::LoadLines(string path)
{
	FILE* filePtr = NULL;
	int filesize = 0;
	if (fopen_s(&filePtr, path.c_str(), "rb") != 0)
		return strings();

	fseek(filePtr, 0, SEEK_END);
	filesize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	chars pBuf;
	pBuf->resize(filesize + 1);
	fread(&pBuf[0], filesize, 1, filePtr);


	strings lines = Split2(&pBuf[0], "\n");

	fclose(filePtr);
	return lines;
}

//example path is like "D:\\temp\\*.*";
void jUtils::ForEachFiles2(void* _object, const char* _path, function<bool(void*, string)> _func)
{
	struct _finddata_t fd;
	intptr_t handle = 1;
	if ((handle = _findfirst(_path, &fd)) >= 0)
	{
		do
		{
			if (strncmp(fd.name, ".", 260) == 0)
				continue;
			if (strncmp(fd.name, "..", 260) == 0)
				continue;

			if (!_func(_object, fd.name))
				break;
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
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
void jUtils::MyCopyFile(string _from, string _to)
{
	jOS_APIs::MyCopyFile(_from, _to);
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
	struct _finddata_t fd;
	intptr_t handle = 1;
	if ((handle = _findfirst(_path, &fd)) >= 0)
	{
		do
		{
			if (strncmp(fd.name, ".", 260) == 0)
				continue;
			if (strncmp(fd.name, "..", 260) == 0)
				continue;

			if (!_func(_object, fd.name))
				break;
		} while (_findnext(handle, &fd) == 0);

		_findclose(handle);
	}
}