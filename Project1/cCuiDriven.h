#pragma once
#include "jComponent.h"
class cCuiDriven :
	public jComponent
{
public:
	cCuiDriven();
	~cCuiDriven();

	function<void(string)> EventCommandInput;

protected:
	virtual void OnStart();
};

