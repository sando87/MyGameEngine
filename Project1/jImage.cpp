#include "jImage.h"
#include "jCaches.h"


jImage::jImage(string fullname)
{
	SetFullname(fullname);
}

jImage::~jImage()
{
}

void jImage::OnLoad()
{
	string fullname = GetFullname();
	
	char* data = (char*)jCaches::CacheClass(fullname, [](string _fullname) {
		string ext = jUtils::GetFileExtension(_fullname);
		chars buf;
		int h, w = 0;
		if (ext == "dump")
			buf = jUtils::LoadFile2(_fullname);
		else
			buf = jUtils::LoadTarga(_fullname, h, w);
	
		char* img = nullptr;
		if (buf)
		{
			int size = buf->size();
			img = new char[size];
			memcpy(img, &buf[0], size);
		}
		return img;
	});
	
	if (data != nullptr)
	{
		mBuffer = data;
		jUtils::ReadTargaSize(fullname, mHeight, mWidth, mBufferSize);
	}
}
