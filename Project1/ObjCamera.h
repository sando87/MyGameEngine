#pragma once
#include "jGameObject.h"
#include "jMatrixControl.h"

class ObjCamera :
	public jGameObject
{
public:
	ObjCamera();
	~ObjCamera();

	Matrix4 getPosMat() { return mPos.getMatrix(); }
	Matrix4 getProjMat() { return mMatProj; }

	void OnStart();
	void OnUpdate();

private:
	double mNear;
	double mFar;
	double mFov;
	double mAspect;  // Width/Height
	Matrix4 mMatProj;
	jMatrixControl mPos;

	void		setProjectionMatrix(double fovDeg, double aspect, double zNear, double zFar);
	//void		setPerspInfo(double fov, double aspect, double zNear, double zFar);
	//void		getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);
	//void		getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);

};

