#include "jImage.h"
#include "jCaches.h"


jImage::jImage(string _name)
{
	mWidth = 0;
	mHeight = 0;
	mFullName = "";
	if (_name.length() > 0)
		Load(_name);
}

jImage::~jImage()
{
}

bool jImage::Load(string name)
{
	string ext = jUtils::GetFileExtension(name);

	if (ext == "dump")
	{
		mFullName = name;
		return true;
	}

	char* data = (char*)jCaches::CacheClass(name, [](string filename) {
		int h = 0;
		int w = 0;
		chars buf = jUtils::LoadTarga(filename, h, w);
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
		mFullName = name;
		jUtils::ReadTargaSize(name, mHeight, mWidth, mBufferSize);
	}
	
	return data == nullptr ? false : true;
}
