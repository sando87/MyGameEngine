#pragma once
#include "jGameObject.h"

class ObjGroundAxis :
	public jGameObject
{
public:
	ObjGroundAxis();
	virtual ~ObjGroundAxis();

private:
	void OnLoad();

};

