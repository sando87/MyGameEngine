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

	Property_Getter(char*, Buffer)
	Property_Getter(int, Width)
	Property_Getter(int, Height)
	Property_Getter(int, BufferSize)
};

