#include "ObjPlayer.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jMesh.h"
#include "jShaderSkin.h"
#include "jMatrixControl.h"
#include "jImage.h"
#include "jTime.h"
#include "jInput.h"
#include "jBoneTree.h"
#include "jOS_APIs.h"
#include "jCrash.h"
#include "jAnimCSV.h"
#include "jLine3D.h"

ObjPlayer::ObjPlayer()
{
}


ObjPlayer::~ObjPlayer()
{
}

void ObjPlayer::OnStart()
{
	LoadTxt("MyObject_397.txt");
	mAnim = FindComponent<jAnimCSV>();
	mShader = FindComponent<jShaderSkin>();

	ShaderParamsSkin& param = mShader->GetParams();
	param.material.diffuse = Vector4f(1, 1, 1, 1);
	param.light.direction = Vector4f(-1, -1, -1, 0);

	mAnim->SetAnimation("idle");

	jInput::GetInst().mMouseDown += [this](jInput::jMouseInfo info)
	{
		Vector2 screenPt = jOS_APIs::GetCursorScreenPos();
		Vector3 view = GetCamera().ScreenToWorldView(screenPt.x, screenPt.y);

		jGameObject* target = jGameObjectMgr::GetInst().RayCast(GetCamera().GetPosture().getPos(), view);
		//Vector3 pt = GetTerrain().CalcGroundPos(GetCamera().GetPosture().getPos(), view);
		jLine3D line3d(GetCamera().GetPosture().getPos(), view);
		Vector2 pt = line3d.GetXY(0);
		WalkTo(pt, target);
	};

	AddComponent((new jCrash())->Init(1, 2, [this](jCrashs objs) {
		if (!objs) return;
		StopCoRoutine("MovePlayer");
		mAnim->SetAnimation("attack", "idle");
	}));

	//CrashCapsule shape;
	//shape.round = 3;
	//shape.height = 10;
	//jCrash* crash = new jCrash();
	//crash->Init(shape, [](jCrashs objs) {
	//	jRect rect = objs[0]->GetRect();
	//	_printlog("ObjPlayer %f\n", rect.Center().x);
	//});
	//AddComponent(crash);
}
void ObjPlayer::OnUpdate()
{
	mat4s mats = mAnim->Animate(jTime::Delta());
	ShaderParamsSkin& param = mShader->GetParams();
	for (int i = 0; i < 45; ++i)
		param.bones[i] = mats[i];

	//Vector3 pos = GetTransport().getPos();
	//float height = 0;
	//bool ret = GetTerrain().GetHeight(pos.x, pos.y, height);
	//if (ret)
	//{
	//	pos.z = height;
	//	GetTransport().moveTo(pos);
	//}
}

void ObjPlayer::WalkTo(Vector2 pos, jGameObject * obj)
{
	mAnim->SetAnimation("walk");
	StartCoRoutine("MovePlayer", [this, obj, pos]() {
		float speed = 10; //1초(60프레임)당 움직이는 속도
		float speedRot = 30; //프레임당 회전하는 속도
		float delta = jTime::Delta();
		Vector3 target = (obj != nullptr) ? obj->GetTransport().getPos() : Vector3(pos.x, pos.y, 0);
		target.z = GetTransport().getPos().z;
		GetTransport().rotateToPos_OnGround(target, speedRot);
		GetTransport().goForward(speed * delta);
		if (GetTransport().getPos().distance(target) < 1)
		{
			mAnim->SetAnimation("idle");
			return CoroutineReturn::Stop;
		}

		return CoroutineReturn::Keep;
	});
}
