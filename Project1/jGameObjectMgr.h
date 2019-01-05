#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include <list>
using namespace std;
class jGameObject;
class jCamera;

class jGameObjectMgr
{
public:
	static jGameObjectMgr& GetInst() { static jGameObjectMgr inst; return inst; }

private:
	jGameObjectMgr();
	~jGameObjectMgr();

	jCamera* mCamera;
	list<jGameObject*> mObjects;

public:
	bool Initialize();
	void Release();
	void RunObjects();

	void AddGameObject(jGameObject* _obj) { mObjects.push_back(_obj); }
	jCamera& GetCamera() { return *mCamera; }
};

#endif