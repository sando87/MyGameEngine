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
#include "jAStar.h"

ObjPlayer::ObjPlayer()
{
	mAstar = new jAStar();
}


ObjPlayer::~ObjPlayer()
{
	delete mAstar;
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
		Vector3 pt = GetTerrain().CalcGroundPos(GetCamera().GetPosture().getPos(), view);
		//jLine3D line3d(GetCamera().GetPosture().getPos(), view);
		//Vector2 pt = line3d.GetXY(0);
		//WalkTo(Vector2(pt.x, pt.y), target);
		StartNavigate(Vector2(pt.x, pt.y));
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

	Vector3 pos = GetTransport().getPos();
	float height = 0;
	bool ret = GetTerrain().GetHeight(pos.x, pos.y, height);
	if (ret)
	{
		pos.z = height;
		GetTransport().moveTo(pos);
	}
}

void ObjPlayer::MoveTo(Vector2 pos)
{
	float speed = 10; //1초(60프레임)당 움직이는 속도
	float speedRot = 30; //프레임당 회전하는 속도
	float delta = jTime::Delta();
	Vector3 target = Vector3(pos.x, pos.y, GetTransport().getPos().z);
	GetTransport().rotateToPos_OnGround(target, speedRot);
	Vector3 nextDir = target - GetTransport().getPos();
	nextDir.normalize();
	Vector3 nextPos = GetTransport().getPos() + (nextDir * speed * delta);
	GetTransport().moveTo(nextPos);
}

void ObjPlayer::WalkTo(Vector2 pos, jGameObject * obj)
{
	mAnim->SetAnimation("walk");
	StartCoRoutine("MovePlayer", [this, obj, pos](CorMember& userData, bool first) {
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
			return CorCmd::Stop;
		}

		return CorCmd::Keep;
	});
}

void ObjPlayer::StartNavigate(Vector2 pos)
{
	mStep = 5;
	StopCoRoutine("NavigatePlayer");
	mAstar->StopRouting();
	mAstar->Moveable = [this](u32 idxX, u32 idxY) {
		float height = 0;
		Vector2 worldPos = Vector2(idxX * mStep, idxY * mStep) + Vector2(mStep / 2, mStep / 2);
		bool ret = GetTerrain().GetHeight(worldPos.x, worldPos.y, height);
		return height > 0 && ret;
	};

	StartCoRoutine("NavigatePlayer",
	[this, pos]() {
		// Route를 수행하여 최적의 경로정보를 산출하는 함수
		u32 startIdxX = (u32)(GetTransport().getPos().x / mStep);
		u32 startIdxY = (u32)(GetTransport().getPos().y / mStep);
		u32 endIdxX = (u32)(pos.x / mStep);
		u32 endIdxY = (u32)(pos.y / mStep);
		mAstar->Route(startIdxX, startIdxY, endIdxX, endIdxY, 1000);
	},
	[this, pos](CorMember& userData, bool first) {
		// Routing결과위치값들을 따라가는 동작 수행
		if (first)
		{
			u32 idxX = (u32)(GetTransport().getPos().x / mStep);
			u32 idxY = (u32)(GetTransport().getPos().y / mStep);
			u64 myPosKey = ToU64(idxX, idxY);
			mAstar->SearchRouteResult(myPosKey);
			if (mAstar->GetResults().size() == 0) //Routing 실패시 아무것도 안함
				return CorCmd::Stop; 

			userData.intVal = mAstar->GetResults().size() - 2;
			StopCoRoutine("MovePlayer");
			mAnim->SetAnimation("walk");
		}
		vector<u64>& rets = mAstar->GetResults();
		if (userData.intVal < 0)
		{
			mAnim->SetAnimation("idle");
			return CorCmd::Stop; //도착지 도달시 정상 종료
		}

		u64 targetKey = rets[userData.intVal];
		Vector2 targetPos = Vector2(ToU32x(targetKey) * mStep, ToU32y(targetKey) * mStep) + Vector2(mStep/2, mStep /2);
		MoveTo(targetPos);

		u32 nextX = (u32)(GetTransport().getPos().x / mStep);
		u32 nextY = (u32)(GetTransport().getPos().y / mStep);
		u64 nextKey = ToU64(nextX, nextY);
		if (nextKey == targetKey)
			userData.intVal--;

		return CorCmd::Keep;
	});

}
