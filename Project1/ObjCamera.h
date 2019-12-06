#pragma once
#include "jGameObject.h"
#include "jMatrixControl.h"
#include "jRect.h"

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
	Vector3 ScreenToWorldView(int _pixelX, int _pixelY);
	jMatrixControl& GetPosture() { return mPos; }
	jRect GetGroundRect() { return mGroundRect; }

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
	jRect mGroundRect;

	void		setProjectionMatrix(int _width, int _height, double fovDeg, double zNear, double zFar);
	jRect		UpdateGroundRect();
	//void		setPerspInfo(double fov, double aspect, double zNear, double zFar);
	//void		getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);
	//void		getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);

};

