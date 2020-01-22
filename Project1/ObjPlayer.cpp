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
#include "jAnimator.h"
#include "jLine3D.h"
#include "jAStar.h"

ObjPlayer::ObjPlayer()
{
	mAstar = new jAStar();
	mTarget = nullptr;
}


ObjPlayer::~ObjPlayer()
{
	delete mAstar;
}

void ObjPlayer::OnStart()
{
	mCamera = &mEngine->GetCamera();
	mTerrain = &mEngine->GetTerrain();
	LoadTxt("MyObject_397.txt");
	mAnim = FindComponent<jAnimator>();
	mAnim->AddEvent("attack", 1.0f, [this]() { mAnim->SetAnimation("idle"); });
	mShader = FindComponent<jShaderSkin>();

	ShaderParamsSkin& param = mShader->GetParams();
	param.material.diffuse = Vector4f(1, 1, 1, 1);
	param.light.direction = Vector4f(-1, -1, -1, 0);

	mAnim->SetAnimation("idle");

	jInput::GetInst().mMouseDown += [this](jInput::jMouseInfo info)
	{
		Vector2 screenPt = jOS_APIs::GetCursorScreenPos();
		Vector3 view = mCamera->ScreenToWorldView(screenPt.x, screenPt.y);

		mTarget = jGameObjectMgr::GetInst().RayCast(mCamera->GetTransport().getPos(), view);

		Vector2 pt;
		bool isVaild = mTerrain->RayCastTerrain(mCamera->GetTransport().getPos(), view, pt);
		if (isVaild == false)
		{
			jLine3D line3d(mCamera->GetTransport().getPos(), view);
			Vector3 tmpPT = line3d.GetXY(GetTransport().getPos().z);
			Vector3 moveStart = GetTransport().getPos();
			moveStart.z = mCamera->GetTransport().getPos().z;
			mTerrain->RayCastTerrain(moveStart, tmpPT - moveStart, pt);
		}
		StartNavigate(Vector2(pt.x, pt.y));
	};

	//AddComponent((new jCrash())->Init(1, 2, [this](jCrashs objs) {
	//	if (!objs) 
	//		return;
	//	mAnim->SetAnimation("attack", "idle");
	//}));
}
void ObjPlayer::OnUpdate()
{
	UpdateComponents();

	FollowWayPoints();
	GoToTarget();

	StandOnTerrain();
}

Vector2 ObjPlayer::MoveTo(Vector2 pos)
{
	double moveSpeed = 20;
	GetTransport().moveSmoothlyToward2D(pos, moveSpeed, jTime::Delta());
	return GetTransport().getPos();
}

void ObjPlayer::StartNavigate(Vector2 pos)
{
	mEngine->StopCoRoutine("NavigatePlayer");
	mAstar->StopRouting();
	mAstar->Moveable = [this](Vector2 worldPos) {
		float height = 0;
		bool ret = mTerrain->GetHeight(worldPos.x, worldPos.y, height);
		return ret;
	};

	mEngine->StartCoRoutine("NavigatePlayer",
	[this, pos]() {
		// Route를 수행하여 최적의 경로정보를 산출하는 함수
		double step = 1;
		mAstar->Route(GetTransport().getPos(), pos, 1000, step);
	},
	[this, pos](CorMember& userData, bool first) {
		// Routing 완료된 결과값들을 가지고 캐릭터를 움직임.
		vector<Vector2>& rets = mAstar->GetResults();
		if (rets.size() == 0) //Routing 실패시 아무것도 안함
			return CorCmd::Stop; 

		Vector3 curPos = GetTransport().getPos();
		mWayPoints.clear();
		OptimizeRouteResults(rets, rets.size(), curPos, mWayPoints);
		
		if(!mWayPoints.empty())
			mAnim->SetAnimation("walk");

		return CorCmd::Stop;
	});

}

void ObjPlayer::FollowWayPoints()
{
	if (mWayPoints.empty())
		return;

	if (mWayPoints.size() == 1 && mTarget != nullptr)
	{
		mWayPoints.clear();
		return;
	}

	Vector2 targetPos = mWayPoints.front();
	Vector2 nextPos = MoveTo(targetPos);
	if (nextPos.distance(targetPos) < 1)
	{
		mWayPoints.pop_front();
		if (mWayPoints.empty())
			mAnim->SetAnimation("idle");
	}
}

void ObjPlayer::GoToTarget()
{
	if (!mWayPoints.empty() || mTarget == nullptr)
		return;
	
	if (mTarget->GetRemove())
	{
		mAnim->SetAnimation("idle");
		mTarget = nullptr;
		return;
	}

	string currentAnim = mAnim->GetAnimation();
	if (currentAnim == "walk")
	{
		Vector2 targetPos = mTarget->GetTransport().getPos();
		Vector2 nextPos = MoveTo(targetPos);
		if (nextPos.distance(targetPos) < 2)
			mAnim->SetAnimation("attack");
	}
	else if (currentAnim == "idle")
	{
		Vector2 targetPos = mTarget->GetTransport().getPos();
		Vector2 curMyPos = GetTransport().getPos();
		if (curMyPos.distance(targetPos) < 2)
			mAnim->SetAnimation("attack");
		else
		{
			MoveTo(targetPos);
			mAnim->SetAnimation("walk");
		}
	}
	else if (currentAnim == "attack")
	{
		//Do Noting...
	}
}

void ObjPlayer::OptimizeRouteResults(vector<Vector2>& inPoints, int count, Vector2 startPos, list<Vector2>& outPoints)
{
	if (count <= 0)
		return;

	Vector2* startData = &inPoints[0];
	int retIdx = jUtils::BinarySearchEdge<Vector2>(startData, count, [this, startPos](Vector2& pt) {
		double step = 1;
		Vector2 pos;
		return mTerrain->Reachable(startPos, pt, pos, step);
	});
	_warnif((retIdx == count - 1))
	int nextIdx = (retIdx == count - 1) ? retIdx : retIdx + 1;
	Vector2 findPos = inPoints[nextIdx];
	outPoints.push_back(findPos);
	OptimizeRouteResults(inPoints, nextIdx, findPos, outPoints);
}
