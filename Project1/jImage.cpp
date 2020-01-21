#include "jImage.h"
#include "jCaches.h"


jImage::jImage(string fullname)
{
	mWidth = 0;
	mHeight = 0;
	mBufferSize = 0;
	mBuffer = nullptr;
	mFullname = fullname;
}

jImage::~jImage()
{
}

void jImage::OnLoad()
{
	string ext = jUtils::GetFileExtension(mFullname);
	if (ext == "dump")
		return;

	char* data = (char*)jCaches::CacheClass(mFullname, [](string fullname) {
		int h = 0;
		int w = 0;
		chars buf = jUtils::LoadTarga(fullname, h, w);
		if (!buf)
			return (char*)nullptr;
	
		int size = buf->size();
		char* img = new char[size];
		memcpy(img, &buf[0], size);
		return img;
	});
	
	if (data != nullptr)
	{
		mBuffer = data;
		jUtils::ReadTargaSize(mFullname, mHeight, mWidth, mBufferSize);
	}
}
