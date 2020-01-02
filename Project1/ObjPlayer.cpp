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

ObjPlayer::ObjPlayer()
{
	mBones = nullptr;
}


ObjPlayer::~ObjPlayer()
{
	if (mBones)
		delete mBones;
}

void ObjPlayer::OnStart()
{
	mBones = new jBoneTree();
	mBones->LoadBoneTreeDAE("mob3.DAE");
	//mBones->LoadAnimateDAE("mob2_idle.DAE", "idle");
	//mBones->LoadAnimateDAE("mob2_run.DAE", "run");
	//mBones->LoadAnimateDAE("mob2_attack.DAE", "attack");

	AddComponent(new jMesh("mob3.DAE"));
	AddComponent(new jImage("./stone01.tga"));
	AddComponent(mShader = new jShaderSkin());
	//mShader->SetEnabled(false);

	Vector4f diffuse = Vector4f(1, 1, 1, 1);
	Vector4f light = Vector4f(-1, -1, -1, 0);
	ShaderParamsSkin& param = mShader->GetParams();
	for(int i = 0; i < 45; ++i)
		param.bones[i] = Matrix4().identity();
	param.material.ambient = diffuse;
	param.material.diffuse = diffuse;
	param.material.shininess = diffuse;
	param.material.specular = diffuse;
	param.light.color = light;
	param.light.direction = light;
	param.light.position = light;
	param.light.reserve = light;

	jInput::GetInst().mMouseDown += [this](jInput::jMouseInfo info)
	{
		Vector2 screenPt = jOS_APIs::GetCursorScreenPos();
		Vector3 view = GetCamera().ScreenToWorldView(screenPt.x, screenPt.y);
		Vector3 pt = GetTerrain().CalcGroundPos(GetCamera().GetPosture().getPos(), view);
	
		StartCoRoutine([this, pt]() {
			float speed = 1; //1초(60프레임)당 움직이는 속도
			float speedRot = 30; //1초(60프레임)당 회전하는 속도
			float delta = jTime::Delta();
			Vector3 target = pt;
			target.z = GetTransport().getPos().z;
			GetTransport().rotateToPos_OnGround(target, speedRot * delta);
			GetTransport().goForward(speed * delta);
			if (GetTransport().getPos().distance(target) < 1)
				return CoroutineStop;
			return CoroutineKeep;
		}, "MovePlayer");
	};

	Vector3 terrainCenter = GetTerrain().GetTerrainCenter();
	//terrainCenter.z = 27;
	GetTransport().moveTo(terrainCenter);

	//CrashCapsule shape;
	//shape.center = Vector3(0, 0, 0);
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
	//Vector3 pos = GetTransport().getPos();
	//float height = 0;
	//bool ret = GetTerrain().GetHeight(pos.x, pos.y, height);
	//if (ret)
	//{
	//	pos.z = height;
	//	GetTransport().moveTo(pos);
	//}
}
void ObjPlayer::SetAnim(string animName)
{
	mBones->SetAnimate(animName);
	StartCoRoutine([this]() {
		mAnimTime += jTime::Delta();
		vector<Matrix4> mats;
		mBones->Animate(mAnimTime, mats);
		ShaderParamsSkin& param = mShader->GetParams();
		for (int i = 0; i < mats.size(); ++i)
			param.bones[i] = mats[i].transpose();
		//GetTransport().goForward(3 * jTime::Delta());
		return CoroutineKeep;
	}, "player");
}
