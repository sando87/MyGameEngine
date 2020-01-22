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

protected:

	Property_Getter(char*, Buffer, nullptr)
	Property_Getter(int, Width, 0)
	Property_Getter(int, Height, 0)
	Property_Getter(int, BufferSize, 0)
};

