#pragma once
#include "jGameObject.h"
#include "jTinyDB.h"

class jUISystem;
class jViewFont;
class ObjPlayer;

class oFormStats :
	public jGameObject
{
public:
	oFormStats();
	virtual ~oFormStats();

protected:
	virtual void OnLoad();
	virtual void OnStart();
	virtual void OnUpdate();

	jUISystem* mForm;
	jViewFont* mlbPA;
	jViewFont* mlbMA;
	jViewFont* mlbPD;
	jViewFont* mlbMD;
	jViewFont* mlbHP;
	jViewFont* mlbMP;
	jViewFont* mlbLevel;
	jViewFont* mlbRemain;

	ObjPlayer* mObjPlayer;
};

