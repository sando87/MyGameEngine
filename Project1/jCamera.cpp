#include <math.h>
#include "jCamera.h"
#include "jMath.h"
#include "jLog.h"
#include "jUtils.h"

jCamera::jCamera()
{
	mMat.identity();
	mMatProj.identity();

	mNear = 0.0;
	mFar = 0.0;
	mFov = 0.0;
	mAspect = 0.0;
}

jCamera::~jCamera()
{
}
void jCamera::init(double fovDeg, double aspect, double zNear, double zFar)
{
	mMat.identity();
	mMatProj.identity();

	setProjectionMatrix(fovDeg, aspect, zNear, zFar);
	lookat(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));
}
jCamera& jCamera::lookat(Vector3 _eye, Vector3 _lookat, Vector3 _up)
{
	Vector3 view = _lookat - _eye;
	Vector3 up = _up;
	Vector3 cross;
	view.normalize();
	up.normalize();
	cross = up.cross(view);
	cross.normalize();
	_warnif(IsOutofOne(cross.length()));
	up = view.cross(cross);
	up.normalize();
	_warnif(IsOutofOne(up.length()));

	mView = view;
	mCross = cross;
	mUp = up;
	mPos = _eye;

	refreshMatrix();
	return (*this);
}
jCamera& jCamera::goForward(double dist)
{
	mPos = mPos + mView*dist;
	refreshMatrix();
	return (*this);
}
jCamera& jCamera::moveTo(Vector3 pos)
{
	mPos = pos;
	refreshMatrix();
	return (*this);
}
jCamera& jCamera::goTo(Vector3 dir)
{
	mPos = mPos + dir;
	refreshMatrix();
	return (*this);
}
jCamera& jCamera::rotateAxis(Vector3 basePoint, Vector3 axis, double degree)
{
	axis.normalize();
	Matrix4 rotMat;
	rotMat.rotate(degree, axis.x, axis.y, axis.z);

	Vector3 newPos = mPos - basePoint;
	Vector3 rotPos = rotMat*newPos;
	Vector3 newUp(0.0, 0.0, 1.0);
	double dotdot = rotPos.dot(newUp);
	newPos = rotPos + basePoint;

	Vector3 cross = newUp.cross(rotPos);
	double dot = cross.dot(mCross);
	if(cross.length()==0 || dot<=0)
	{
		newUp = rotMat*mUp;
		newUp.z = 0.0;
		newUp.normalize();
		newPos.x = basePoint.x;
		newPos.y = basePoint.y;
		newPos.z = rotPos.length();
		lookat(newPos, basePoint, newUp);
	}
	else
		lookat(newPos, basePoint, newUp);

	return (*this);
}
void jCamera::refreshMatrix()
{
	mMat.identity();
	mMat.setColumn(0, mCross);
	mMat.setColumn(1, mUp);
	mMat.setColumn(2, mView);
	mMat[12] = -mPos.dot(mCross);
	mMat[13] = -mPos.dot(mUp);
	mMat[14] = -mPos.dot(mView);
}
void jCamera::refreshAxis()
{
	Vector3 cross(mMat[0], mMat[4], mMat[8]);
	Vector3 up(mMat[1], mMat[5], mMat[9]);
	Vector3 view(-mMat[2], -mMat[6], -mMat[10]);
	cross.normalize();
	up.normalize();
	view.normalize();
	_warnif(IsOutofOne(cross.cross(view).dot(up)));

	Matrix4 mat;
	mat.setColumn(0, cross);
	mat.setColumn(1, up);
	mat.setColumn(2, -view);
	mat.invert();
	Matrix4 posMat = mMat * mat;
	mPos = Vector3(-posMat[12], -posMat[13], -posMat[14]);
	mCross = cross;
	mUp = up;
	mView = view;
	refreshMatrix();
}


void jCamera::setProjectionMatrix(double fovDeg, double aspect, double zNear, double zFar)
{
	jUtils::GetPerspectiveFovLH(mMatProj, fovDeg, aspect, zNear, zFar);

	mNear = zNear;
	mFar = zFar;
	mFov = fovDeg;
	mAspect = aspect;
}

/*
void jCamera::setPerspInfo(double fov, double aspect, double zNear, double zFar)
{
	mAspect = aspect;
	mNear = zNear;
	mFar = zFar;
	if( mAspect<1.0 )
	{
		double rad_w = fov*DegToRad;
		double rad_h = atan( tan(rad_w)/mAspect );
		mFov = rad_h*RadToDeg;
	}
	else
		mFov = fov;
}
void jCamera::getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar)
{
	zNear = (float)mNear;
	zFar = (float)mFar;
	top = (float)mNear*(float)tan(mFov*DegToRad);
	bottom = -(float)top;
	left = bottom*(float)mAspect;  
	right = top*(float)mAspect;
}
void jCamera::getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar)
{
	float h_half = (float)mPos.z * (float)tan(mFov*DegToRad);
	top = h_half;
	bottom = -top;
	left = bottom*(float)mAspect;  
	right = top*(float)mAspect;
	zNear = (float)mNear;
	zFar = (float)mFar;
}
*/

//OpenGL좌표계 (Cross:X, View:-Z, Up:Y) , Projection Mat은 고정
//DirectX좌표계 (Cross:X, View:Z, Up:Y), Projection Mat은 고정

/*
//RightHand (Cross:X, View:Y, Up:Z) To DirectX Projection Mat
jCamera& jCamera::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
{
	Vector3 view = _lookat - _eye;
	Vector3 up = _up;
	Vector3 cross;
	view.normalize();
	up.normalize();
	cross = view.cross(up);
	cross.normalize();
	warnif(IsOutofOne(cross.length()));
	up = cross.cross(view);
	up.normalize();
	warnif(IsOutofOne(up.length()));

	mView = view;
	mCross = cross;
	mUp = up;
	mPos = _eye;

	mMat.identity();
	mMat.setColumn(0, mCross);
	mMat.setColumn(1, mUp);
	mMat.setColumn(2, mView);
	mMat[12] = -mPos.dot(mCross);
	mMat[13] = -mPos.dot(mUp);
	mMat[14] = -mPos.dot(mView);

	return (*this);
}

//LeftHand (Cross:X, View:Z, Up:Y) To DirectX Projection Mat
jCamera& jCamera::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
{
	Vector3 view = _lookat - _eye;
	Vector3 up = _up;
	Vector3 cross;
	view.normalize();
	up.normalize();
	cross = up.cross(view);
	cross.normalize();
	warnif(IsOutofOne(cross.length()));
	up = view.cross(cross);
	up.normalize();
	warnif(IsOutofOne(up.length()));

	mView = view;
	mCross = cross;
	mUp = up;
	mPos = _eye;

	mMat.identity();
	mMat.setColumn(0, mCross);
	mMat.setColumn(1, mUp);
	mMat.setColumn(2, mView);
	mMat[12] = -mPos.dot(mCross);
	mMat[13] = -mPos.dot(mUp);
	mMat[14] = -mPos.dot(mView);

	return (*this);
}

//RightHand (Cross:X, View:Y, Up:Z) To OpenGL Projection Mat
jCamera& jCamera::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
{
	Vector3 view = _lookat - _eye;
	Vector3 up = _up;
	Vector3 cross;
	view.normalize();
	up.normalize();
	cross = view.cross(up);
	cross.normalize();
	warnif(IsOutofOne(cross.length()));
	up = cross.cross(view);
	up.normalize();
	warnif(IsOutofOne(up.length()));

	mView = view;
	mCross = cross;
	mUp = up;
	mPos = _eye;

	mMat.identity();
	mMat.setColumn(0, mCross);
	mMat.setColumn(1, mUp);
	mMat.setColumn(2, -mView);
	mMat[12] = -mPos.dot(mCross);
	mMat[13] = -mPos.dot(mUp);
	mMat[14] = mPos.dot(mView);

	return (*this);
}

*/