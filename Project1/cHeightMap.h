#pragma once
#include "jComponent.h"

class cHeightMap :
	public jComponent
{
public:
	cHeightMap() {}
	~cHeightMap() {}
protected:
	virtual void OnStart();

	void LoadHeightMapFromMesh();
};

