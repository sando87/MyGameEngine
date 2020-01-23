#ifndef __EZ_CAMERA_H__
#define __EZ_CAMERA_H__

#include "junks.h"
#include "jComponent.h"

class jTransform : public jComponent
{
private :
	Vector3 mView;
	Vector3 mCross;
	Vector3 mUp;
	Vector3 mPos;

public :
	jTransform();
	virtual ~jTransform();

	Vector3		getView() { return mView; }
	Vector3		getUp() { return mUp; }
	Vector3		getCross() { return mCross; }
	Vector3		getPos() { return mPos; }

	Matrix4		refreshMatrix();
	void			refreshAxis(Matrix4 _mat);
	Matrix4		getLocalMatrix();
	Matrix4		getWorldMatrix();

	jTransform&		lookat(Vector3 eye, Vector3 lookat, Vector3 up);
	jTransform&		lookDir(Vector3 view);
	jTransform&		lookPos(Vector3 pos);
	jTransform&		goForward(double dist);
	jTransform&		moveTo(Vector3 pos);
	jTransform&		goTo(Vector3 dir);
	jTransform&		rotateAxis(Vector3 groundPt, Vector3 axisUP, double degree);
	jTransform&		moveSmoothlyToward2D(Vector2 pos, double moveSpeed, double dt);
};

#endif	// __EZ_CAMERA_H__
