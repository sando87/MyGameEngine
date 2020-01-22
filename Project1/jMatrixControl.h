#ifndef __EZ_CAMERA_H__
#define __EZ_CAMERA_H__

#include "junks.h"

class jMatrixControl
{
private :
	Vector3 mView;
	Vector3 mCross;
	Vector3 mUp;
	Vector3 mPos;

public :
	jMatrixControl();
	virtual ~jMatrixControl();

	Matrix4		getMatrix() { return refreshMatrix(); }
	Vector3		getView() { return mView; }
	Vector3		getUp() { return mUp; }
	Vector3		getCross() { return mCross; }
	Vector3		getPos() { return mPos; }

	Matrix4		refreshMatrix();
	void		refreshAxis(Matrix4 _mat);

	jMatrixControl&		lookat(Vector3 eye, Vector3 lookat, Vector3 up);
	jMatrixControl&		lookDir(Vector3 view);
	jMatrixControl&		lookPos(Vector3 pos);
	jMatrixControl&		goForward(double dist);
	jMatrixControl&		moveTo(Vector3 pos);
	jMatrixControl&		goTo(Vector3 dir);
	jMatrixControl&		rotateAxis(Vector3 groundPt, Vector3 axisUP, double degree);
	jMatrixControl&		moveSmoothlyToward2D(Vector2 pos, double moveSpeed, double dt);
};

#endif	// __EZ_CAMERA_H__
