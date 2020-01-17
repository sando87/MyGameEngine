#pragma once
#include "jGameObject.h"
#include "junks.h"

class jShaderSkin;
class jAnimCSV;
class jAStar;

class ObjPlayer : public jGameObject
{
public:
	ObjPlayer();
	virtual ~ObjPlayer();

private:
	virtual void OnStart();
	virtual void OnUpdate();

	Vector2 MoveTo(Vector2 pos);
	void WalkTo(Vector2 pos, jGameObject * obj);
	void StartNavigate(Vector2 pos);
	void UpdatePlayerPosition();
	void OptimizeRouteResults(vector<Vector2>& inPoints, int startIdx, Vector2 startPos, list<Vector2>& outPoints);

	jShaderSkin* mShader;
	jAnimCSV* mAnim;
	jAStar* mAstar;

	list<Vector2> mWayPoints;
	float mAnimTime = 0;
};

