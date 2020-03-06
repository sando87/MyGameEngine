#pragma once
#include "jComponent.h"
#include "junks.h"

class jImage :
	public jComponent
{
public:
	jImage(string fullname = "");
	virtual ~jImage();

	virtual void OnLoad();
	bool LoadImgFile();
	
protected:


	Property_Getter(chars, Buffer, chars())
	Property_Getter(int, Width, 0)
	Property_Getter(int, Height, 0)
};

