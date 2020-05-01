#pragma once
#include "cUserInputDriven.h"

class jUISystem;

class jEventForm :
	public cUserInputDriven
{
public:
	jEventForm(jUISystem *form, char toggleKey);
	virtual ~jEventForm();

protected:
	jUISystem * mForm;
	char mToggleKey;
};

