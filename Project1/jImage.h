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
	//char* GetBuffer() { return &(mBuffer[0]); }
	char* GetBuffer() { return mBuffer; }

protected:
	char* mBuffer;
	//chars mBuffer;


	Property_Getter(int, Width)
	Property_Getter(int, Height)
	Property_Getter(int, BufferSize)
	Property_Getter(string, FileName)
};

