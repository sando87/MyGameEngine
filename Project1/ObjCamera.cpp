#include "ObjCamera.h"
#include "junks.h"
#include "jLine3D.h"
#include "jGameObjectMgr.h"
#include "jRenderer.h"

//INITIALIZER(camera)
//{
//	jGameObjectMgr::GetInst().AddGameObject(new ObjCamera());
//}

ObjCamera::ObjCamera()
{
	mPlayer = nullptr;
}


ObjCamera::~ObjCamera()
{
}

void ObjCamera::OnLoad()
{
	cUserInputDriven* eventCam = new cUserInputDriven();
	eventCam->EventMouseDrag = [&](InputEventArgs args) { 
		OnMouseDrag(args.deltaPt, args.type); 
		return EventResult::TransferEvent; 
	};
	eventCam->EventMouseWheel = [&](InputEventArgs args) { 
		OnMouseWheel(args.delta); 
		return EventResult::TransferEvent; 
	};
	AddComponent(eventCam);
}

void ObjCamera::OnStart()
{
	int screenWidth = jRenderer::GetInst().GetScreenWidth();
	int screenHeight = jRenderer::GetInst().GetScreenHeight();
	setProjectionMatrix(screenWidth, screenHeight, 45, 1.0, 1000.0);

	mPlayer = GetEngine().FindGameObject("ObjPlayer");
	if(mPlayer == nullptr)
		//GetTransform().lookat(Vector3(2340, 4860, 0) + Vector3(35, 35, 50), Vector3(2340, 4860, 0), Vector3(0, 0, 1));
		GetTransform().lookat(Vector3(0, 0, 0) + Vector3(35, 35, 50), Vector3(0, 0, 0), Vector3(0, 0, 1));
	else
	{
		Vector3 pos = mPlayer->GetTransform().getPos();
		GetTransform().lookat(pos + Vector3(35, 35, 50), pos, Vector3(0, 0, 1));
	}
}
void ObjCamera::OnUpdate()
{
	mGroundRect = UpdateGroundRect();
	//Vector2 center = mGroundRect.GetCenter();
	//static int iii = 0;
	//iii++;
	//if (iii % 180 == 0)
	//	_printlog("cen : %f, %f\n", center.x, center.y);
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
void ObjCamera::setOrthogonalMatrix(double _left, double _right, double _bottom, double _top, double _near, double _far)
{
	mIsOrthogonal = true;
	mWidth = _right - _left;
	mHeight = _top - _bottom;
	GetOrthogonalMat(mMatProj, _left, _right, _bottom, _top, _near, _far);

	mNear = _near;
	mFar = _far;
	mAspect = (double)mWidth / mHeight;
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
		return GetTransform().getView();

	double wh = mWidth / 2;
	double hh = mHeight / 2;
	double pixelRateX = (_pixelX - wh) / wh;
	double pixelRateY = (hh - _pixelY) / hh;
	double width_half = tan(DegToRad(mFovDegHori*0.5));
	double height_half = width_half / mAspect;
	Vector3 dirA = GetTransform().getView() + width_half * pixelRateX * GetTransform().getCross() + height_half * pixelRateY * GetTransform().getUp();
	dirA.normalize();
	return dirA;
}
Vector3 ObjCamera::GetViewOnMouse(int _x, int _y)
{
	Vector4 pt(0.5, 0, 0.999998, 1);
	Vector4 camPt = pt * getProjMat().invert();
	Vector4 projPt = camPt * getPosMat_D3D().invert();
	Vector3 worldPt(projPt.x / projPt.w, projPt.y / projPt.w, projPt.z / projPt.w);
	Vector3 view = worldPt - GetTransform().getPos();

	return view.normalize();
}

jRectangle2D ObjCamera::UpdateGroundRect()
{
	jRectangle2D rt;
	Vector3 view = GetTransform().getView();
	Vector3 cross = GetTransform().getCross();
	Vector3 up = GetTransform().getUp();
	Vector3 pos = GetTransform().getPos();
	if (mIsOrthogonal)
	{
		double width_half = mOrthRect.Size().x * 0.5;
		double height_half = mOrthRect.Size().y * 0.5;
		Vector3 posA = pos + width_half * cross + height_half * up;
		Vector3 posB = pos + width_half * cross - height_half * up;
		Vector3 posC = pos - width_half * cross + height_half * up;
		Vector3 posD = pos - width_half * cross - height_half * up;
		Vector2 gptA = jLine3D(posA, posA + view).GetXY(0);
		Vector2 gptB = jLine3D(posB, posB + view).GetXY(0);
		Vector2 gptC = jLine3D(posC, posC + view).GetXY(0);
		Vector2 gptD = jLine3D(posD, posD + view).GetXY(0);
		double minX = min(min(min(gptA.x, gptB.x), gptC.x), gptD.x);
		double minY = min(min(min(gptA.y, gptB.y), gptC.y), gptD.y);
		double maxX = max(max(max(gptA.x, gptB.x), gptC.x), gptD.x);
		double maxY = max(max(max(gptA.y, gptB.y), gptC.y), gptD.y);
		rt.SetMinMax(Vector2(minX, minY), Vector2(maxX, maxY));
		rt.ClipMinus();
	}
	else
	{
		double width_half = tan(DegToRad(mFovDegHori*0.5));
		double height_half = width_half / mAspect;
		Vector3 dirA = view + width_half * cross + height_half * up;
		Vector3 dirB = view + width_half * cross - height_half * up;
		Vector3 dirC = view - width_half * cross + height_half * up;
		Vector3 dirD = view - width_half * cross - height_half * up;
		Vector2 gptA = jLine3D(pos, pos + dirA).GetXY(0);
		Vector2 gptB = jLine3D(pos, pos + dirB).GetXY(0);
		Vector2 gptC = jLine3D(pos, pos + dirC).GetXY(0);
		Vector2 gptD = jLine3D(pos, pos + dirD).GetXY(0);
		double minX = min(min(min(gptA.x, gptB.x), gptC.x), gptD.x);
		double minY = min(min(min(gptA.y, gptB.y), gptC.y), gptD.y);
		double maxX = max(max(max(gptA.x, gptB.x), gptC.x), gptD.x);
		double maxY = max(max(max(gptA.y, gptB.y), gptC.y), gptD.y);
		rt.SetMinMax(Vector2(minX, minY), Vector2(maxX, maxY));
		rt.ClipMinus();
	}
	return rt;
}

Matrix4 ObjCamera::getPosMat_D3D()
{
	Matrix4 mat;
	Vector3 pos = GetTransform().getPos();
	Vector3 cross = GetTransform().getCross();
	Vector3 view = GetTransform().getView();
	Vector3 up = GetTransform().getUp();
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
	Vector3 pos = GetTransform().getPos();
	Vector3 cross = GetTransform().getCross();
	Vector3 view = GetTransform().getView();
	Vector3 up = GetTransform().getUp();
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
	float h_half = (float)GetTransform().z * (float)tan(mFov*DegToRad);
	top = h_half;
	bottom = -top;
	left = bottom*(float)mAspect;
	right = top*(float)mAspect;
	zNear = (float)mNear;
	zFar = (float)mFar;
}
*/


void ObjCamera::OnMouseDrag(Vector2n delta, MouseButtonType type)
{
	jTransform& trans = GetTransform();
	Vector3 pos = trans.getPos();
	Vector3 view = trans.getView();
	
	if (MouseButtonType::Center == type)
	{
		jLine3D line(pos, pos + view);
		Vector3 pos = line.GetXY(0);
		trans.rotateAxis(Vector3(pos.x, pos.y, 0.0f), Vector3(0, 0, 1), delta.x);
	}
	else if (MouseButtonType::Left == type)
	{
		float sensX = -delta.x * 0.01f * pos.z;
		float sensY = delta.y * 0.01f * pos.z;
		Vector3 cross = trans.getCross();
		Vector3 up = trans.getUp();
		up.z = 0;
		cross.normalize();
		up.normalize();
		Vector3 newPos = pos + (cross * sensX) + (up * sensY);
		trans.moveTo(newPos);
	}
}

void ObjCamera::OnMouseWheel(int delta)
{
	if (delta > 0)
		GetTransform().goForward(10.0f);
	else if (delta < 0)
		GetTransform().goForward(-10.0f);
}
