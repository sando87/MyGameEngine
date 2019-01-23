#pragma once
#include "jGameObject.h"
#include "jMatrixControl.h"

class ObjCamera :
	public jGameObject
{
public:
	ObjCamera();
	~ObjCamera();

	Matrix4 getPosMat_D3D();
	Matrix4 getPosMat_GL();
	Matrix4 getProjMat() { return mMatProj; }

	void OnStart();
	void OnUpdate();
	Vector3 GetViewOnMouse(int _x, int _y);
	jMatrixControl& GetPosture() { return mPos; }

private:
	int mWidth;
	int mHeight;
	double mNear;
	double mFar;
	double mFovDegHori;
	double mFovDegVerti;
	double mAspect;  // Width/Height
	Matrix4 mMatProj;
	jMatrixControl mPos;

	void		setProjectionMatrix(int _width, int _height, double fovDeg, double zNear, double zFar);
	//void		setPerspInfo(double fov, double aspect, double zNear, double zFar);
	//void		getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);
	//void		getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);

};

