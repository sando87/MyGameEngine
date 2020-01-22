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
	string ext = jUtils::GetFileExtension(fullname);
	if (ext == "dump")
		return;

	char* data = (char*)jCaches::CacheClass(fullname, [](string fullname) {
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
		jUtils::ReadTargaSize(fullname, mHeight, mWidth, mBufferSize);
	}
}
