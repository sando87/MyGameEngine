#pragma once
#include "jGameObject.h"
#include "jTransform.h"
#include "jRect3D.h"

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
	jRect GetGroundRect() { return mGroundRect; }

	void		setProjectionMatrix(int _width, int _height, double fovDeg, double zNear, double zFar);
	void		setOrthogonalMatrix(int _width, int _height, double _left, double _right, double _bottom, double _top, double _near, double _far);

private:
	int mWidth;
	int mHeight;
	double mNear;
	double mFar;
	double mFovDegHori;
	double mFovDegVerti;
	double mAspect;  // Width/Height
	bool mIsOrthogonal;
	Matrix4 mMatProj;
	jRect mGroundRect;
	jRect3D mOrthRect;
	jGameObject * mPlayer;

	void		GetPerspectiveFovLH(Matrix4& _mat, double _fovDeg, double _aspect, double _near, double _far);
	void		GetOrthogonalMat(Matrix4& _mat, double _left, double _right, double _bottom, double _top, double _near, double _far);
	jRect		UpdateGroundRect();
	//void		setPerspInfo(double fov, double aspect, double zNear, double zFar);
	//void		getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);
	//void		getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar);

};

