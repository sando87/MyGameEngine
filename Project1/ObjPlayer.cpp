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

	UpdatePlayerPosition();

	Vector3 pos = GetTransport().getPos();
	float height = 0;
	bool ret = GetTerrain().GetHeight(pos.x, pos.y, height);
	if (ret)
	{
		pos.z = height;
		GetTransport().moveTo(pos);
	}
}

Vector2 ObjPlayer::MoveTo(Vector2 pos)
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
	return nextPos;
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
	StopCoRoutine("NavigatePlayer");
	mAstar->StopRouting();
	mAstar->Moveable = [this](Vector2 worldPos) {
		float height = 0;
		bool ret = GetTerrain().GetHeight(worldPos.x, worldPos.y, height);
		return ret;
	};

	StartCoRoutine("NavigatePlayer",
	[this, pos]() {
		// Route를 수행하여 최적의 경로정보를 산출하는 함수
		double step = 1;
		mAstar->Route(GetTransport().getPos(), pos, 1000, step);
	},
	[this, pos](CorMember& userData, bool first) {
		// Routing 완료된 결과값들을 가지고 캐릭터를 움직임.
		if (first)
		{
			mAstar->SearchRouteResult(GetTransport().getPos(), 3.0);
			vector<Vector2>& rets = mAstar->GetResults();
			if (rets.size() == 0) //Routing 실패시 아무것도 안함
				return CorCmd::Stop; 

			Vector3 curPos = GetTransport().getPos();
			OptimizeRouteResults(rets, rets.size(), curPos, mWayPoints);
			
			userData.intVal = mAstar->GetResults().size() - 2;
			StopCoRoutine("MovePlayer");
			mAnim->SetAnimation("walk");
		}

		vector<Vector2>& rets = mAstar->GetResults();
		if (userData.intVal < 0)
		{
			mAnim->SetAnimation("idle");
			return CorCmd::Stop; //도착지 도달시 정상 종료
		}

		Vector2 targetPos = rets[userData.intVal];
		MoveTo(targetPos);

		Vector2 myPos = GetTransport().getPos();
		if (myPos.distance(targetPos) < 0.5)
			userData.intVal--;

		return CorCmd::Keep;
	});

}

void ObjPlayer::UpdatePlayerPosition()
{
	if (mWayPoints.empty())
		return;

	Vector2 targetPos = mWayPoints.front();
	Vector2 nextPos = MoveTo(targetPos);
	if (nextPos.distance(targetPos) < 1)
	{
		mWayPoints.pop_front();
		if (mWayPoints.empty())
			mAnim->SetAnimation("idle");
	}
}

void ObjPlayer::OptimizeRouteResults(vector<Vector2>& inPoints, int count, Vector2 startPos, list<Vector2>& outPoints)
{
	if (count <= 0)
		return;

	Vector2* startData = &inPoints[0];
	int retIdx = jUtils::BinarySearchEdge<Vector2>(startData, count, [this, startPos](Vector2& pt) {
		double step = 1;
		return GetTerrain().Reachable(startPos, pt, Vector2(), step);
	});
	_warnif((retIdx == count - 1))
	int nextIdx = (retIdx == count - 1) ? retIdx : retIdx + 1;
	Vector2 findPos = inPoints[nextIdx];
	outPoints.push_front(findPos);
	OptimizeRouteResults(inPoints, nextIdx, findPos, outPoints);
}
