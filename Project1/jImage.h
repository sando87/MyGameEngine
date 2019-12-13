#pragma once
#include "jComponent.h"
#include "junks.h"

class jImage :
	public jComponent
{
public:
	jImage(string _name = "");
	virtual ~jImage();

	bool Load(string name);
	int GetBufferSize() { return mBuffer->size(); }
	char* GetBuffer() { return &mBuffer[0]; }

protected:
	chars mBuffer;


	Property_Getter(int, Width)
	Property_Getter(int, Height)
	Property_Getter(string, FileName)
};

