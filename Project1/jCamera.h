#ifndef __EZ_CAMERA_H__
#define __EZ_CAMERA_H__

#include "Matrices.h"

class jCamera
{
private :
	Vector3 mView;
	Vector3 mCross;
	Vector3 mUp;
	Vector3 mPos;
	Matrix4 mMat;
	Matrix4 mMatProj;

	double mNear;
	double mFar;
	double mFov;
	double mAspect;  // Width/Height

public :
	jCamera();
	virtual ~jCamera();

	void			init(double fovDeg, double aspect, double zNear, double zFar);

	Matrix4		getProjectionMat() { return mMatProj; }
	Matrix4		getMatrix() { return mMat; }
	Vector3		getView() { return mView; }
	Vector3		getUp() { return mUp; }
	Vector3		getCross() { return mCross; }
	Vector3		getPos() { return mPos; }

	double		getZNear() { return mNear; }
	double		getZFar() { return mFar; }
	double		getFov() { return mFov; }
	double		getAspect() { return mAspect; }

	void		setProjectionMatrix(double fovDeg, double aspect, double zNear, double zFar);
	//void		setPerspInfo(double fov, double aspect, double zNear, double zFar);
	//void		getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);
	//void		getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);

	void		refreshMatrix();
	void		refreshAxis();

	jCamera&		lookat(Vector3 eye, Vector3 lookat, Vector3 up);
	jCamera&		goForward(double dist);
	jCamera&		moveTo(Vector3 pos);
	jCamera&		goTo(Vector3 dir);
	jCamera&		rotateAxis(Vector3 basePoint, Vector3 axis, double degree);
};

#endif	// __EZ_CAMERA_H__
