#include "ObjCamera.h"
#include "junks.h"
#include "jLine3D.h"
#include "jInput.h"
#include "ObjTerrainMgr.h"

ObjCamera::ObjCamera()
{
}


ObjCamera::~ObjCamera()
{
}
void ObjCamera::setProjectionMatrix(int _width, int _height, double fovDeg, double zNear, double zFar)
{
	mWidth = _width;
	mHeight = _height;
	double aspect = (double)_width / _height;
	GetPerspectiveFovLH(mMatProj, fovDeg, aspect, zNear, zFar);

	mNear = zNear;
	mFar = zFar;
	mFovDegHori = fovDeg;
	mAspect = aspect;
	double mFovRadVerti = atan(tan(DegToRad(fovDeg*0.5)) / aspect);
	mFovDegVerti = RadToDeg(mFovRadVerti);
	mFovDegVerti *= 2;
}
void ObjCamera::GetPerspectiveFovLH(Matrix4& _mat, double _fovDeg, double _aspect, double _near, double _far)
{
	_errorif(_near == _far || _aspect == 0);

	_mat.identity();
	_mat[5] = 1 / tan(DegToRad(_fovDeg * 0.5));
	_mat[0] = _mat[5] / _aspect;
	_mat[10] = _far / (_far - _near);
	_mat[11] = 1.0;
	_mat[14] = (_far*_near) / (_near - _far);
	_mat[15] = 0.0;
}

void ObjCamera::OnStart()
{
	setProjectionMatrix(640, 480, 45, 1.0, 1000.0);
	Vector3 terrainCenter = GetTerrain().GetTerrainCenter();
	mPos.lookat(Vector3(terrainCenter.x +25, terrainCenter.y +25, 50), Vector3(terrainCenter.x, terrainCenter.y, 0), Vector3(0, 0, 1));
	jInput::GetInst().mMouse += [&](auto info)
	{
		if (info.z > 0)
			mPos.goForward(10.0f);
		else if(info.z < 0)
			mPos.goForward(-10.0f);
	
		if (info.middle & 0x80 && info.x != 0)
		{
			jLine3D line(mPos.getPos(), mPos.getView());
			Position2 pos = line.GetXY(0);
			mPos.rotateAxis(Vector3(pos.x, pos.y, 0.0f), Vector3(0, 0, 1), info.x);
		}

		if (info.left & 0x80)
		{
			if (info.x != 0)
			{
				float sensX = -info.x * 0.01f * mPos.getPos().z;
				Vector3 cross = mPos.getCross();
				cross.normalize();
				Vector3 newPos = mPos.getPos() + (cross * sensX);
				mPos.moveTo(newPos);
			}
			if (info.y != 0)
			{
				float sensY = info.y * 0.01f * mPos.getPos().z;
				Vector3 up = mPos.getUp();
				up.z = 0;
				up.normalize();
				Vector3 newPos = mPos.getPos() + (up * sensY);
				mPos.moveTo(newPos);
			}
		}
	};
}
void ObjCamera::OnUpdate()
{
	mGroundRect = UpdateGroundRect();
	Vector2 center = mGroundRect.Center();
	static int iii = 0;
	iii++;
	if(iii % 180 == 0)
		_printlog("cen : %f, %f\n", center.x, center.y);
}

Vector3 ObjCamera::ScreenToWorldView(int _pixelX, int _pixelY)
{
	double wh = 640 / 2;
	double hh = 480 / 2;
	double pixelRateX = (_pixelX - wh) / wh;
	double pixelRateY = (hh - _pixelY) / hh;
	double width_half = tan(DegToRad(mFovDegHori*0.5));
	double height_half = width_half / mAspect;
	Vector3 dirA = mPos.getView() + width_half * pixelRateX * mPos.getCross() + height_half * pixelRateY * mPos.getUp();
	dirA.normalize();
	return dirA;
}
Vector3 ObjCamera::GetViewOnMouse(int _x, int _y)
{
	Position4 pt(0.5, 0, 0.999998, 1);
	Position4 camPt = pt * getProjMat().invert();
	Position4 projPt = camPt * getPosMat_D3D().invert();
	Position3 worldPt(projPt.x / projPt.w, projPt.y / projPt.w, projPt.z / projPt.w);
	Vector3 view = worldPt - mPos.getPos();

	return view.normalize();
}

jRect ObjCamera::UpdateGroundRect()
{
	double width_half = tan(DegToRad(mFovDegHori*0.5));
	double height_half = width_half / mAspect;
	Vector3 dirA = mPos.getView() + width_half * mPos.getCross() + height_half * mPos.getUp();
	jLine3D lineA(mPos.getPos(), dirA);
	Vector2 gptA = lineA.GetXY(0);
	Vector3 dirB = mPos.getView() + width_half * mPos.getCross() - height_half * mPos.getUp();
	jLine3D lineB(mPos.getPos(), dirB);
	Vector2 gptB = lineB.GetXY(0);
	Vector3 dirC = mPos.getView() - width_half * mPos.getCross() + height_half * mPos.getUp();
	jLine3D lineC(mPos.getPos(), dirC);
	Vector2 gptC = lineC.GetXY(0);
	Vector3 dirD = mPos.getView() - width_half * mPos.getCross() - height_half * mPos.getUp();
	jLine3D lineD(mPos.getPos(), dirD);
	Vector2 gptD = lineD.GetXY(0);
	jRect rt;
	rt.expand(gptA).expand(gptB).expand(gptC).expand(gptD);
	rt.ClipMinus();
	return rt;
}

Matrix4 ObjCamera::getPosMat_D3D()
{
	Matrix4 mat;
	Vector3 pos = mPos.getPos();
	Vector3 cross = mPos.getCross();
	Vector3 view = mPos.getView();
	Vector3 up = mPos.getUp();
	mat.identity();
	mat.setColumn(0, cross);
	mat.setColumn(1, up);
	mat.setColumn(2, view);
	mat[12] = -pos.dot(cross);
	mat[13] = -pos.dot(up);
	mat[14] = -pos.dot(view);
	return mat;
}
Matrix4 ObjCamera::getPosMat_GL()
{
	Matrix4 mat;
	Vector3 pos = mPos.getPos();
	Vector3 cross = mPos.getCross();
	Vector3 view = mPos.getView();
	Vector3 up = mPos.getUp();
	mat.identity();
	mat.setColumn(0, cross);
	mat.setColumn(1, up);
	mat.setColumn(2, -view);
	mat[12] = -pos.dot(cross);
	mat[13] = -pos.dot(up);
	mat[14] = pos.dot(view);
	return mat;
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
