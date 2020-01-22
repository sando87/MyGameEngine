#include "ObjCamera.h"
#include "junks.h"
#include "jLine3D.h"
#include "jInput.h"
#include "jGameObjectMgr.h"

INITIALIZER(camera)
{
	jGameObjectMgr::GetInst().AddGameObject(new ObjCamera());
}

ObjCamera::ObjCamera()
{
	mPlayer = nullptr;
}


ObjCamera::~ObjCamera()
{
}

void ObjCamera::OnStart()
{
	mPlayer = mEngine->FindGameObject("ObjPlayer");
	Vector3 pos = mPlayer->GetTransport().getPos();
	setProjectionMatrix(640, 480, 45, 1.0, 1000.0);
	GetTransport().lookat(pos + Vector3(25, 25, 50), pos, Vector3(0, 0, 1));
	jInput::GetInst().mMouse += [&](auto info)
	{
		if (info.z > 0)
			GetTransport().goForward(10.0f);
		else if (info.z < 0)
			GetTransport().goForward(-10.0f);

		if (info.middle & 0x80 && info.x != 0)
		{
			jLine3D line(GetTransport().getPos(), GetTransport().getView());
			Vector3 pos = line.GetXY(0);
			GetTransport().rotateAxis(Vector3(pos.x, pos.y, 0.0f), Vector3(0, 0, 1), info.x);
		}

		if (info.left & 0x80)
		{
			if (info.x != 0)
			{
				float sensX = -info.x * 0.01f * GetTransport().getPos().z;
				Vector3 cross = GetTransport().getCross();
				cross.normalize();
				Vector3 newPos = GetTransport().getPos() + (cross * sensX);
				GetTransport().moveTo(newPos);
			}
			if (info.y != 0)
			{
				float sensY = info.y * 0.01f * GetTransport().getPos().z;
				Vector3 up = GetTransport().getUp();
				up.z = 0;
				up.normalize();
				Vector3 newPos = GetTransport().getPos() + (up * sensY);
				GetTransport().moveTo(newPos);
			}
		}
	};
}
void ObjCamera::OnUpdate()
{
	Vector3 pos = mPlayer->GetTransport().getPos();
	GetTransport().moveTo(pos + Vector3(25, 25, 50));

	mGroundRect = UpdateGroundRect();
	Vector2 center = mGroundRect.Center();
	static int iii = 0;
	iii++;
	if (iii % 180 == 0)
		_printlog("cen : %f, %f\n", center.x, center.y);
}

void ObjCamera::setProjectionMatrix(int _width, int _height, double fovDeg, double zNear, double zFar)
{
	mIsOrthogonal = false;
	mWidth = _width;
	mHeight = _height;
	double aspect = (double)_width / _height;
	GetPerspectiveFovLH(mMatProj, fovDeg, aspect, zNear, zFar);

	mNear = zNear;
	mFar = zFar;
	mFovDegVerti = fovDeg;
	mAspect = aspect;
	double fovRadVerti = atan(tan(DegToRad(fovDeg*0.5)) * aspect);
	mFovDegHori = RadToDeg(fovRadVerti);
	mFovDegHori *= 2;
}
void ObjCamera::setOrthogonalMatrix(int _width, int _height, double _left, double _right, double _bottom, double _top, double _near, double _far)
{
	mIsOrthogonal = true;
	mWidth = _width;
	mHeight = _height;
	GetOrthogonalMat(mMatProj, _left, _right, _bottom, _top, _near, _far);

	mNear = _near;
	mFar = _far;
	mAspect = (double)_width / _height;
	mFovDegVerti = 0;
	mFovDegHori = 0;

	Vector3 pos = Vector3(_left, _bottom, _near);
	Vector3 size = Vector3(_right, _top, _far) - pos;
	mOrthRect.SetRect(pos, size);
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
void ObjCamera::GetOrthogonalMat(Matrix4& _mat, double _left, double _right, double _bottom, double _top, double _near, double _far)
{
	_mat.identity();
	_mat[0] = 2 / (_right - _left);
	_mat[12] = -(_right + _left) / (_right - _left);

	_mat[5] = 2 / (_top - _bottom);
	_mat[13] = -(_top + _bottom) / (_top - _bottom);


	_mat[10] = 1 / (_far - _near); //DirectX case
	_mat[14] = -(_near) / (_far - _near); //DirectX case

	//_mat[10] = -2 / (_far - _near); //Opengl case
	//_mat[14] = -(_far + _near) / (_far - _near); //Opengl case

	_mat[15] = 1;
}

Vector3 ObjCamera::ScreenToWorldView(int _pixelX, int _pixelY)
{
	if (mIsOrthogonal)
		return GetTransport().getView();

	double wh = (mWidth - 17) / 2; //Window 좌우 경계 픽셀 제외
	double hh = (mHeight - 40) / 2; //window title 및 하단 경계 픽셀 제외
	double pixelRateX = (_pixelX - wh) / wh;
	double pixelRateY = (hh - _pixelY) / hh;
	double width_half = tan(DegToRad(mFovDegHori*0.5));
	double height_half = width_half / mAspect;
	Vector3 dirA = GetTransport().getView() + width_half * pixelRateX * GetTransport().getCross() + height_half * pixelRateY * GetTransport().getUp();
	dirA.normalize();
	return dirA;
}
Vector3 ObjCamera::GetViewOnMouse(int _x, int _y)
{
	Vector4 pt(0.5, 0, 0.999998, 1);
	Vector4 camPt = pt * getProjMat().invert();
	Vector4 projPt = camPt * getPosMat_D3D().invert();
	Vector3 worldPt(projPt.x / projPt.w, projPt.y / projPt.w, projPt.z / projPt.w);
	Vector3 view = worldPt - GetTransport().getPos();

	return view.normalize();
}

jRect ObjCamera::UpdateGroundRect()
{
	jRect rt;
	if (mIsOrthogonal)
	{
		double width_half = mOrthRect.Size().x * 0.5;
		double height_half = mOrthRect.Size().y * 0.5;
		Vector3 posA = GetTransport().getPos() + width_half * GetTransport().getCross() + height_half * GetTransport().getUp();
		Vector3 posB = GetTransport().getPos() + width_half * GetTransport().getCross() - height_half * GetTransport().getUp();
		Vector3 posC = GetTransport().getPos() - width_half * GetTransport().getCross() + height_half * GetTransport().getUp();
		Vector3 posD = GetTransport().getPos() - width_half * GetTransport().getCross() - height_half * GetTransport().getUp();
		Vector2 gptA = jLine3D(posA, GetTransport().getView()).GetXY(0);
		Vector2 gptB = jLine3D(posB, GetTransport().getView()).GetXY(0);
		Vector2 gptC = jLine3D(posC, GetTransport().getView()).GetXY(0);
		Vector2 gptD = jLine3D(posD, GetTransport().getView()).GetXY(0);
		rt.expand(gptA).expand(gptB).expand(gptC).expand(gptD);
		rt.ClipMinus();
	}
	else
	{
		double width_half = tan(DegToRad(mFovDegHori*0.5));
		double height_half = width_half / mAspect;
		Vector3 dirA = GetTransport().getView() + width_half * GetTransport().getCross() + height_half * GetTransport().getUp();
		Vector3 dirB = GetTransport().getView() + width_half * GetTransport().getCross() - height_half * GetTransport().getUp();
		Vector3 dirC = GetTransport().getView() - width_half * GetTransport().getCross() + height_half * GetTransport().getUp();
		Vector3 dirD = GetTransport().getView() - width_half * GetTransport().getCross() - height_half * GetTransport().getUp();
		Vector2 gptA = jLine3D(GetTransport().getPos(), dirA).GetXY(0);
		Vector2 gptB = jLine3D(GetTransport().getPos(), dirB).GetXY(0);
		Vector2 gptC = jLine3D(GetTransport().getPos(), dirC).GetXY(0);
		Vector2 gptD = jLine3D(GetTransport().getPos(), dirD).GetXY(0);
		rt.expand(gptA).expand(gptB).expand(gptC).expand(gptD);
		rt.ClipMinus();
	}
	return rt;
}

Matrix4 ObjCamera::getPosMat_D3D()
{
	Matrix4 mat;
	Vector3 pos = GetTransport().getPos();
	Vector3 cross = GetTransport().getCross();
	Vector3 view = GetTransport().getView();
	Vector3 up = GetTransport().getUp();
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
	Vector3 pos = GetTransport().getPos();
	Vector3 cross = GetTransport().getCross();
	Vector3 view = GetTransport().getView();
	Vector3 up = GetTransport().getUp();
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
	float h_half = (float)GetTransport().z * (float)tan(mFov*DegToRad);
	top = h_half;
	bottom = -top;
	left = bottom*(float)mAspect;
	right = top*(float)mAspect;
	zNear = (float)mNear;
	zFar = (float)mFar;
}
*/
