#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRect3D.h"

class jCrash;

enum ShapeType {
	NoShape, Capsule, Point, Line,  Area
};
struct ShapeCrash
{
	ShapeType type;
	double round;
	Vector3 posA;
	Vector3 posB;
	Vector3 posC;
	double height;
	jRect3D GetRect()
	{
		Vector3 min = Vector3(-round, -round, 0);
		Vector3 max = Vector3(round, round, round * 2 + height);
		return jRect3D(posA + min, max - min);
	}
	Vector3 GetCenter()
	{
		Vector3 base = posA;
		base.z += round + (height * 0.5);
		return base;
	}
};

struct CrashInfo
{
	bool crashed;
	double dist;
	jCrash* target;
};

typedef shared_ptr_array<jCrash*> jCrashes;
class jCrash :
	public jComponent
{
public:
	jCrash();
	virtual ~jCrash();

	void SetShape(double round, double height) { mShape.type = ShapeType::Capsule; mShape.round = round; mShape.height = height; }

	jRect3D GetRect();
	Vector3 GetCenter();
	double GetRound() { return mShape.round; }


	bool IsCrash(jLine3D line);


	static void Clear();

protected:
	virtual void OnLoad();
	virtual void OnUpdate();
	virtual void OnCollision(CrashInfo info) {}

	ShapeCrash mShape;


	static unordered_multimap<u64, jCrash*> mCrashGrid;
	static CrashInfo CheckCrashed(jCrash* left, jCrash* right);
	static void GetGrids(jRect rect, vector<pair<u64, jCrashes>>& grids);
};

