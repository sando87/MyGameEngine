#include <math.h>
#include "jMatrixControl.h"
#include "jMath.h"
#include "jLog.h"
#include "jUtils.h"

jMatrixControl::jMatrixControl()
{
	mMat.identity();
}

jMatrixControl::~jMatrixControl()
{
}
void jMatrixControl::init()
{
	mMat.identity();
	lookat(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));
}
jMatrixControl& jMatrixControl::lookat(Vector3 _eye, Vector3 _lookat, Vector3 _up)
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
jMatrixControl& jMatrixControl::goForward(double dist)
{
	mPos = mPos + mView*dist;
	refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::moveTo(Vector3 pos)
{
	mPos = pos;
	refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::goTo(Vector3 dir)
{
	mPos = mPos + dir;
	refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::rotateAxis(Vector3 groundPt, Vector3 axisUP, double degree)
{
	axisUP.normalize();
	Matrix4 rotMat;
	rotMat.rotate(degree, axisUP.x, axisUP.y, axisUP.z);

	Vector3 newPos = mPos - groundPt;
	Vector3 rotPos = rotMat*newPos;
	newPos = rotPos + groundPt;

	lookat(newPos, groundPt, axisUP);

	return (*this);
}
void jMatrixControl::refreshMatrix()
{
	mMat.identity();
	mMat.setColumn(0, mCross);
	mMat.setColumn(1, mUp);
	mMat.setColumn(2, mView);
	mMat[12] = -mPos.dot(mCross);
	mMat[13] = -mPos.dot(mUp);
	mMat[14] = -mPos.dot(mView);
}
void jMatrixControl::refreshAxis()
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


//OpenGL좌표계 (Cross:X, View:-Z, Up:Y) , Projection Mat은 고정
//DirectX좌표계 (Cross:X, View:Z, Up:Y), Projection Mat은 고정

/*
//RightHand (Cross:X, View:Y, Up:Z) To DirectX Projection Mat
jMatrixControl& jMatrixControl::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
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
jMatrixControl& jMatrixControl::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
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
jMatrixControl& jMatrixControl::lookat2(Vector3 _eye, Vector3 _lookat, Vector3 _up)
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