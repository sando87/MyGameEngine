#include "jMatrixControl.h"

jMatrixControl::jMatrixControl()
{
}

jMatrixControl::~jMatrixControl()
{
}
void jMatrixControl::init()
{
	lookat(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
}
jMatrixControl& jMatrixControl::lookDir(Vector3 view)
{
	lookat(mPos, mPos + view, Vector3(0,0,1));
	return (*this);
}
jMatrixControl& jMatrixControl::lookPos(Vector3 pos)
{
	lookat(mPos, pos, Vector3(0, 0, 1));
	return (*this);
}
jMatrixControl& jMatrixControl::lookat(Vector3 _eye, Vector3 _lookat, Vector3 _up)
{
	Vector3 view = _lookat - _eye;
	Vector3 up = _up;
	Vector3 cross;
	view.normalize();
	up.normalize();
	cross = view.cross(up);
	cross.normalize();
	_warnif(IsOutofOne(cross.length()));
	up = cross.cross(view);
	up.normalize();
	_warnif(IsOutofOne(up.length()));

	mView = view;
	mCross = cross;
	mUp = up;
	mPos = _eye;

	//refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::goForward(double dist)
{
	mPos = mPos + mView*dist;
	//refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::moveTo(Vector3 pos)
{
	mPos = pos;
	//refreshMatrix();
	return (*this);
}
jMatrixControl& jMatrixControl::goTo(Vector3 dir)
{
	mPos = mPos + dir;
	//refreshMatrix();
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
jMatrixControl& jMatrixControl::rotateToPos_OnGround(Vector3 pos, double degree)
{
	pos.z = mPos.z;
	Vector3 targetView = pos - mPos;
	targetView.normalize();
	Vector3 turnDir = mView.cross(targetView);
	if (turnDir.length() < 0.0001f)
		return (*this);

	turnDir.normalize();
	Matrix4 rotMat;
	rotMat.rotate(degree, 0, 0, turnDir.z);
	mView = rotMat * mView;
	float after = mView.cross(targetView).z;
	if (turnDir.z * after < 0)
		mView = targetView;
	mCross = mView.cross(mUp);
	mCross.normalize();
	return (*this);
}
Matrix4 jMatrixControl::refreshMatrix()
{
	Matrix4 mat;
	mat.identity();
	mat.setRow(0, mCross);
	mat.setRow(1, mView);
	mat.setRow(2, mUp);
	mat.setRow(3, mPos);
	return mat;
}

void jMatrixControl::refreshAxis(Matrix4 _mat)
{
	Vector3 mCross(_mat[0], _mat[1], _mat[2]);
	Vector3 mView(_mat[4], _mat[5], _mat[6]);
	Vector3 mUp(_mat[8], _mat[9], _mat[10]);
	Vector3 mPos(_mat[12], _mat[13], _mat[14]);
	mCross.normalize();
	mView.normalize();
	mUp.normalize();
}


/*
void jMatrixControl::refreshAxis()
{
	Vector3 cross(mMat[0], mMat[4], mMat[8]);
	Vector3 view(mMat[1], mMat[5], mMat[9]);
	Vector3 up(-mMat[2], -mMat[6], -mMat[10]);
	cross.normalize();
	up.normalize();
	view.normalize();
	_warnif(IsOutofOne(cross.cross(view).dot(up)));

	Matrix4 mat;
	mat.setColumn(0, cross);
	mat.setColumn(1, view);
	mat.setColumn(2, up);
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