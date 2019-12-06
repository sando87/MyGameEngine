#ifndef __JGAMEOBJECT_H__
#define __JGAMEOBJECT_H__

class jGameObjectMgr;
class ObjCamera;
class ObjTerrainMgr;

class jGameObject
{
	friend class jGameObjectMgr;

public:
	jGameObject();
	virtual ~jGameObject();

	void AddToMgr();
	ObjCamera& GetCamera();
	ObjTerrainMgr& GetTerrain();
	void DeleteFromMgr() { mIsRemoved = true; }

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnDraw();

	bool mIsRemoved;
	bool mIsStarted;
};

#endif