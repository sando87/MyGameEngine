#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRect3D.h"
#include "jGrid.h"

class jCrash;

enum ShapeType {
	NoShape, Capsule, Point, Line,  Area
};
struct ShapeBase
{
	ShapeType type;
	double round;
	virtual jRect3D GetRect(Vector3 parentPos) 
	{
		Vector3 min = Vector3(-round, -round, -round);
		Vector3 max = Vector3(round, round, round);
		return jRect3D(parentPos, max - min);
	}
	virtual Vector3 GetCenter()
	{
		return Vector3();
	}
};
struct ShapeCapsule : ShapeBase
{
	Vector3 pos;
	double height;
	virtual jRect3D GetRect(Vector3 parentPos)
	{
		Vector3 min = Vector3(-round, -round, 0);
		Vector3 max = Vector3(round, round, round*2 + height);
		return jRect3D(pos + parentPos, max - min);
	}
	virtual Vector3 GetCenter()
	{
		return Vector3(pos.x, pos.y, pos.z + round + (height * 0.5));
	}
};
struct ShapePoint : ShapeBase
{
	Vector3 pos;
};
struct ShapeLine : ShapeBase
{
	Vector3 posA;
	Vector3 posB;
};
struct ShapeArea : ShapeBase
{
	Vector3 posA;
	Vector3 posB;
	Vector3 posC;
};

struct CrashInfo
{
	bool crashed;
	jCrash* target;
};
struct CrashTrigInfo
{
	bool isNew;
	bool isKeep;
	CrashInfo info;
};
typedef shared_ptr_array<CrashInfo> jCrashInfos;
class jCrash :
	public jComponent
{
public:
	jCrash();
	virtual ~jCrash();

	//friend void jCrash::AddCrashedResult(CrashInfo result);

	void SetShape(ShapeBase* shape) { mShape = shape; }
	void SetEventEnter(function<void(jCrashInfos)> event) { mCallbacksEnter = event; }
	void SetEventKeep(function<void(jCrashInfos)> event) { mCallbacksKeep = event; }
	void SetEventLeave(function<void(jCrashInfos)> event) { mCallbacksLeave = event; }

	bool IsCrash(jLine3D line);

	virtual void OnLoad();
	virtual void OnAddToGrid();
	virtual void OnUpdate();

private:
	static CrashInfo CheckCrashed(jCrash* left, jCrash* right);
	void AddCrashedResult(CrashInfo result);
	void CallbackCrashedObjects();
	jRect3D GetRect();

	ShapeBase* mShape;
	unordered_map<jCrash*, CrashTrigInfo> mCrashedList;
	function<void(jCrashInfos)> mCallbacksEnter;
	function<void(jCrashInfos)> mCallbacksKeep;
	function<void(jCrashInfos)> mCallbacksLeave;

	jGrid<list<jCrash*>>* mCrashGrid;
};

