#ifndef __JGAMEOBJECTMGR_H__
#define __JGAMEOBJECTMGR_H__

#include <list>
#include <utility>
#include <unordered_map>
using namespace std;
class jGameObject;
class ObjCamera;
struct MyResBase;

class jGameObjectMgr
{
public:
	static jGameObjectMgr& GetInst() { static jGameObjectMgr inst; return inst; }

private:
	jGameObjectMgr();
	~jGameObjectMgr();

	ObjCamera* mCamera;
	list<jGameObject*> mObjects;
	

public:
	bool Initialize();
	void Release();
	void RunObjects();

	bool AddGPURes(MyResBase* pData, void* pInterface);
	bool InitResources(int _idx);
	void AddResource(MyResBase* _res);
	MyResBase* GetResource(void* _res);

	void AddGameObject(jGameObject* _obj) { mObjects.push_back(_obj); }
	ObjCamera& GetCamera() { return *mCamera; }

	unordered_map<void *, MyResBase*> mResources;
	unordered_map<void *, pair<MyResBase*, void *>> mGPURes;
	int mFileIndex;
};

#endif