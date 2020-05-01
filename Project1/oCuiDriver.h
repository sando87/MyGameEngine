#pragma once
#include "jGameObject.h"

class cCuiDriven;

class oCuiDriver :
	public jGameObject
{
public:
	oCuiDriver();
	~oCuiDriver();

	void AddDriven(cCuiDriven* driven) { mDrivens.push_back(driven); }
	void SubDriven(cCuiDriven* driven) { mDrivens.remove(driven); }
protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	thread mThread;
	list<cCuiDriven*> mDrivens;
	string mCommandString = "";
	bool mUserInput = false;
};

