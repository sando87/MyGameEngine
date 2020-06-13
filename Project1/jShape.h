#pragma once
#include "junks.h"
#include "jRect3D.h"
#include "jGridContainer.h"

class cCollider;

enum class ShapeType {
	None = 0,
	Sphere = 0x1,
	Cylinder = 0x2,
	Box = 0x4,
	Line = 0x8,
	GridCylinder = 0x10,
};

struct CrashResult
{
	bool isCrash;
	Vector3 posFrom;
	Vector3 posTo;
	Vector3 dir;
	double dist;
	cCollider* from;
	cCollider* to;
	cCollider* target;
	CrashResult() { memset(this, 0x00, sizeof(CrashResult)); }
};

class jShapeSphere;
class jShapeCylinder;
class jShapeLine;
class jShapeGridCylinder;

class jShape
{
public:
	virtual jRect3D GetBox() = 0;
	virtual CrashResult IsCrash(jShape* shape) = 0;

	cCollider * Collider;
	ShapeType Type;

	static CrashResult IsCrashSphSph(jShapeSphere* sphereA, jShapeSphere* sphereB);
	static CrashResult IsCrashSphCyl(jShapeSphere* sphere, jShapeCylinder* cylinder);
	static CrashResult IsCrashSphLine(jShapeSphere* sphere, jShapeLine* line);
	static CrashResult IsCrashCylCyl(jShapeCylinder* cylinderA, jShapeCylinder* cylinderB);
	static CrashResult IsCrashCylLine(jShapeCylinder* cylinder, jShapeLine* line);
	static CrashResult IsCrashGCSph(jShapeGridCylinder* shapes, jShapeSphere* sphere);
	static CrashResult IsCrashGCCyl(jShapeGridCylinder* shapes, jShapeCylinder* cylinder);
	static CrashResult IsCrashGCLine(jShapeGridCylinder* shapes, jShapeLine* line);
};


class jShapeSphere : public jShape
{
public:
	jShapeSphere() { Type = ShapeType::Sphere; }

	virtual jRect3D GetBox();
	virtual CrashResult IsCrash(jShape* shape);

	Vector3 Position;
	double Round;
};

class jShapeCylinder : public jShape
{
public:
	jShapeCylinder() { Type = ShapeType::Cylinder; }

	virtual jRect3D GetBox();
	virtual CrashResult IsCrash(jShape* shape);
	double GetHeight() { return PositionTop.z - PositionBottom.z; }

	Vector3 PositionBottom;
	Vector3 PositionTop;
	double Round;
};

class jShapeLine : public jShape
{
public:
	jShapeLine() { Type = ShapeType::Line; }

	virtual jRect3D GetBox();
	virtual CrashResult IsCrash(jShape* shape);

	Vector3 PositionStart;
	Vector3 PositionEnd;
};

class jShapeGridCylinder : public jShape
{
public:
	jShapeGridCylinder() { Type = ShapeType::GridCylinder; }

	virtual jRect3D GetBox();
	virtual CrashResult IsCrash(jShape* shape);

	jRect3D Box;
	jGridContainer<jShapeCylinder> Shapes;
};
