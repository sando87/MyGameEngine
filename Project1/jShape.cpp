#include "jShape.h"
#include "cCollider.h"

CrashResult jShape::IsCrashSphSph(jShapeSphere* sphereA, jShapeSphere* sphereB)
{
	_exceptif(sphereA->Type != ShapeType::Sphere || sphereB->Type != ShapeType::Sphere, return CrashResult());
	double dist = sphereA->Position.distance(sphereB->Position);
	if (dist > sphereA->Round + sphereB->Round)
		return CrashResult();

	CrashResult ret;
	ret.dist = dist;
	ret.posFrom = sphereA->Position;
	ret.posTo = sphereB->Position;
	ret.dir = (sphereB->Position - sphereA->Position).normalize();
	ret.isCrash = true;
	ret.from = sphereA->Parent;
	ret.to = sphereB->Parent;
	return ret;
}

CrashResult jShape::IsCrashSphCyl(jShapeSphere* sphere, jShapeCylinder* cylinder)
{
	_exceptif(sphere->Type != ShapeType::Sphere || cylinder->Type != ShapeType::Cylinder, return CrashResult());
	double roundSum = sphere->Round + cylinder->Round;
	if (Vector2(sphere->Position).distance(Vector2(cylinder->PositionBottom)) > roundSum
		|| sphere->Position.z < cylinder->PositionBottom.z - roundSum || cylinder->PositionTop.z + roundSum < sphere->Position.z)
		return CrashResult();

	CrashResult ret;
	ret.isCrash = true;
	ret.posFrom = sphere->Position;
	ret.posTo = cylinder->PositionBottom;
	ret.posTo.z = sphere->Position.z;
	ret.from = sphere->Parent;
	ret.to = cylinder->Parent;
	return ret;
}

CrashResult jShape::IsCrashSphLine(jShapeSphere* sphere, jShapeLine* line)
{
	_exceptif(sphere->Type != ShapeType::Sphere || line->Type != ShapeType::Line, return CrashResult());
	jLine3D line3D(line->PositionStart, line->PositionEnd);
	Vector3 ptOnLine = line3D.ClosePoint(sphere->Position);
	double dist = ptOnLine.distance(sphere->Position);
	if (dist > sphere->Round || !line3D.IsContains(ptOnLine))
		return CrashResult();

	CrashResult ret;
	ret.isCrash = true;
	ret.posFrom = sphere->Position;
	ret.posTo = ptOnLine;
	ret.dist = dist;
	ret.dir = ptOnLine - sphere->Position;
	ret.from = sphere->Parent;
	ret.to = line->Parent;
	return ret;
}

CrashResult jShape::IsCrashCylCyl(jShapeCylinder * cylinderA, jShapeCylinder * cylinderB)
{
	_exceptif(cylinderA->Type != ShapeType::Cylinder || cylinderB->Type != ShapeType::Cylinder, return CrashResult());
	Vector2 DistAtoB = Vector2(cylinderB->PositionBottom) - Vector2(cylinderA->PositionBottom);
	if (DistAtoB.length() > (cylinderA->Round + cylinderB->Round)
		|| cylinderA->PositionBottom.z > cylinderB->PositionTop.z 
		|| cylinderB->PositionBottom.z > cylinderA->PositionTop.z)
		return CrashResult();

	CrashResult ret;
	ret.isCrash = true;
	ret.dist = DistAtoB.length();
	ret.dir = DistAtoB;
	ret.from = cylinderA->Parent;
	ret.to = cylinderB->Parent;
	return ret;
}

CrashResult jShape::IsCrashCylLine(jShapeCylinder * cylinder, jShapeLine * line)
{
	_exceptif(cylinder->Type != ShapeType::Cylinder || line->Type != ShapeType::Line, return CrashResult());
	jLine3D lnCylinder(cylinder->PositionBottom, cylinder->PositionTop);
	jLine3D lnLine(line->PositionStart, line->PositionEnd);
	Vector3 ptOnCylinder;
	Vector3 ptOnLine;
	double dist = lnCylinder.Distance(lnLine, ptOnCylinder, ptOnLine);
	if (dist > cylinder->Round || !lnCylinder.IsContains(ptOnCylinder, cylinder->Round) || !lnLine.IsContains(ptOnLine, cylinder->Round))
		return CrashResult();

	CrashResult ret;
	ret.isCrash = true;
	ret.posFrom = ptOnCylinder;
	ret.posTo = ptOnLine;
	ret.dist = dist;
	ret.dir = ptOnLine - ptOnCylinder;
	ret.from = cylinder->Parent;
	ret.to = line->Parent;
	return ret;
}

CrashResult jShape::IsCrashGCSph(jShapeGridCylinder * shapes, jShapeSphere * sphere)
{
	_exceptif(shapes->Type != ShapeType::GridCylinder || sphere->Type != ShapeType::Sphere, return CrashResult());
	jRectangle2D rect = sphere->GetBox().Top();
	vector<Vector2> points;
	rect.ClipGrid(shapes->Shapes.GetStep(), points);
	for (Vector2 pt : points)
	{
		if (shapes->Shapes.IsContains(pt))
		{
			jShapeCylinder& gridCyl = shapes->Shapes[pt];
			CrashResult ret = jShape::IsCrashSphCyl(sphere, &gridCyl);
			if (ret.isCrash)
				return ret;
		}
	}
	return CrashResult();
}

CrashResult jShape::IsCrashGCCyl(jShapeGridCylinder * shapes, jShapeCylinder * cylinder)
{
	_exceptif(shapes->Type != ShapeType::GridCylinder || cylinder->Type != ShapeType::Cylinder, return CrashResult());
	jRectangle2D rect = cylinder->GetBox().Top();
	vector<Vector2> points;
	rect.ClipGrid(shapes->Shapes.GetStep(), points);
	for (Vector2 pt : points)
	{
		if (shapes->Shapes.IsContains(pt))
		{
			jShapeCylinder& gridCyl = shapes->Shapes[pt];
			CrashResult ret = jShape::IsCrashCylCyl(cylinder, &gridCyl);
			if (ret.isCrash)
				return ret;
		}
	}
	return CrashResult();
}

CrashResult jShape::IsCrashGCLine(jShapeGridCylinder * shapes, jShapeLine * line)
{
	_exceptif(shapes->Type != ShapeType::GridCylinder || line->Type != ShapeType::Line, return CrashResult());
	for (auto cyl : shapes->Shapes.GetGrids())
	{
		jShapeCylinder& gridCyl = cyl.second;
		CrashResult ret = jShape::IsCrashCylLine(&gridCyl, line);
		if (ret.isCrash)
			return ret;
	}
	return CrashResult();
}

jRect3D jShapeSphere::GetBox()
{
	return jRect3D(Position - Vector3(Round, Round, Round), Vector3(Round * 2, Round * 2, Round * 2));
}

CrashResult jShapeSphere::IsCrash(jShape * shape)
{
	switch (shape->Type)
	{
	case ShapeType::Sphere:
		return jShape::IsCrashSphSph(this, (jShapeSphere*)shape);
	case ShapeType::Cylinder:
		return jShape::IsCrashSphCyl(this, (jShapeCylinder*)shape);
	case ShapeType::Line:
		return jShape::IsCrashSphLine(this, (jShapeLine*)shape);
	case ShapeType::GridCylinder:
		return jShape::IsCrashGCSph((jShapeGridCylinder*)shape, this);
	default:
		_exceptif(true, break);
	}
	return CrashResult();
}

jRect3D jShapeCylinder::GetBox()
{
	return jRect3D(PositionBottom - Vector3(Round, Round, 0), Vector3(Round * 2, Round * 2, PositionTop.z - PositionBottom.z));
}

CrashResult jShapeCylinder::IsCrash(jShape * shape)
{
	switch (shape->Type)
	{
	case ShapeType::Sphere:
		return jShape::IsCrashSphCyl((jShapeSphere*)shape, this);
	case ShapeType::Cylinder:
		return jShape::IsCrashCylCyl(this, (jShapeCylinder*)shape);
	case ShapeType::Line:
		return jShape::IsCrashCylLine(this, (jShapeLine*)shape);
	case ShapeType::GridCylinder:
		return jShape::IsCrashGCCyl((jShapeGridCylinder*)shape, this);
	default:
		_exceptif(true, break);
	}
	return CrashResult();
}

jRect3D jShapeLine::GetBox()
{
	Vector3 minPos;
	Vector3 maxPos;
	minPos.x = min(PositionStart.x, PositionEnd.x);
	minPos.y = min(PositionStart.y, PositionEnd.y);
	minPos.z = min(PositionStart.z, PositionEnd.z);
	maxPos.x = max(PositionStart.x, PositionEnd.x);
	maxPos.y = max(PositionStart.y, PositionEnd.y);
	maxPos.z = max(PositionStart.z, PositionEnd.z);
	return jRect3D(minPos, maxPos - minPos);
}

CrashResult jShapeLine::IsCrash(jShape * shape)
{
	switch (shape->Type)
	{
	case ShapeType::Sphere:
		return jShape::IsCrashSphLine((jShapeSphere*)shape, this);
	case ShapeType::Cylinder:
		return jShape::IsCrashCylLine((jShapeCylinder*)shape, this);
	case ShapeType::GridCylinder:
		return jShape::IsCrashGCLine((jShapeGridCylinder*)shape, this);
	default:
		_exceptif(true, break);
	}
	return CrashResult();
}

jRect3D jShapeGridCylinder::GetBox()
{
	return Box;
}

CrashResult jShapeGridCylinder::IsCrash(jShape * shape)
{
	switch (shape->Type)
	{
	case ShapeType::Sphere:
		return jShape::IsCrashGCSph(this, (jShapeSphere*)shape);
	case ShapeType::Cylinder:
		return jShape::IsCrashGCCyl(this, (jShapeCylinder*)shape);
	case ShapeType::Line:
		return jShape::IsCrashGCLine(this, (jShapeLine*)shape);
	case ShapeType::GridCylinder:
		return CrashResult();
	default:
		_exceptif(true, break);
	}
	return CrashResult();
}

