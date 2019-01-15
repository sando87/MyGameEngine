#include "ObjCamera.h"
#include "jUtils.h"
#include "jInput.h"

ObjCamera::ObjCamera()
{
}


ObjCamera::~ObjCamera()
{
}
void ObjCamera::setProjectionMatrix(double fovDeg, double aspect, double zNear, double zFar)
{
	jUtils::GetPerspectiveFovLH(mMatProj, fovDeg, aspect, zNear, zFar);

	mNear = zNear;
	mFar = zFar;
	mFov = fovDeg;
	mAspect = aspect;
}

void ObjCamera::OnStart()
{
	setProjectionMatrix(45, 640 / 480, 1.0, 1000.0);
	mPos.lookat(Vector3(-200, 200, -200), Vector3(0, 0, 0), Vector3(0, 1, 0));
	jInput::GetInst().mMouse = [&](auto info)
	{
		if (info.z > 0)
			mPos.goForward(10.0f);
		else if(info.z < 0)
			mPos.goForward(-10.0f);

		if (info.middle & 0x80 && info.x != 0)
		{
			mPos.rotateAxis(Vector3(0, 0, 0), Vector3(0, 1, 0), info.x);
		}
	};
}
void ObjCamera::OnUpdate()
{
}

/*
void ObjCamera::setPerspInfo(double fov, double aspect, double zNear, double zFar)
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
void ObjCamera::getFrustumInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar)
{
	zNear = (float)mNear;
	zFar = (float)mFar;
	top = (float)mNear*(float)tan(mFov*DegToRad);
	bottom = -(float)top;
	left = bottom*(float)mAspect;
	right = top*(float)mAspect;
}
void ObjCamera::getOrthofInfo(float& left, float& right, float& bottom, float& top, float& zNear, float& zFar)
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
