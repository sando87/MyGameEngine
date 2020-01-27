#include "jNavigator.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jAStar.h"
#include "jGameObject.h"
#include "jTime.h"

jNavigator::jNavigator()
{
}


jNavigator::~jNavigator()
{
}

void jNavigator::OnLoad()
{
	mCamera = GetEngine().FindGameObject<ObjCamera>();
	mTerrain = GetEngine().FindGameObject<ObjTerrainMgr>();
	mAstar.Moveable = [this](Vector2 worldPos) {
		float height = 0;
		bool ret = mTerrain->GetHeight(worldPos.x, worldPos.y, height);
		return ret;
	};
}

void jNavigator::OnUpdate()
{
	//if (mWayPoints.empty())
	//	return;
	//
	//Vector2 targetPos = mWayPoints.front();
	//GetGameObject()->GetTransform().moveSmoothlyToward2D(targetPos, mMoveSpeed, jTime::Delta());
	//if (GetGameObject()->GetTransform().getPos().distance(targetPos) < 2)
	//{
	//	mWayPoints.pop_front();
	//	if (mWayPoints.empty())
	//		mIsDone = true;
	//}
}

void jNavigator::StartNavigate(Vector2 destPos, function<void(list<Vector2>&)> waypoints)
{
	GetEngine().StopCoRoutine("NavigatePlayer");
	mAstar.StopRouting();
	

	GetEngine().StartCoRoutine("NavigatePlayer",
		[this, destPos]() {
		// Route를 수행하여 최적의 경로정보를 산출하는 함수
		double step = 1;
		mAstar.Route(GetGameObject()->GetTransform().getPos(), destPos, 1000, step);
	},
		[this, waypoints](CorMember& userData, bool first) {
		// Routing 완료된 결과값들을 가지고 캐릭터를 움직임.
		vector<Vector2>& rets = mAstar.GetResults();
		if (rets.size() == 0) //Routing 실패시 아무것도 안함
			return CorCmd::Stop;

		Vector3 curPos = GetGameObject()->GetTransform().getPos();
		list<Vector2> wayPts;
		OptimizeRouteResults(rets, rets.size(), curPos, wayPts);
		waypoints(wayPts);
		return CorCmd::Stop;
	});
}
void jNavigator::OptimizeRouteResults(vector<Vector2>& inPoints, int count, Vector2 startPos, list<Vector2>& outPoints)
{
	if (count <= 0)
		return;

	Vector2* startData = &inPoints[0];
	int retIdx = jUtils::BinarySearchEdge<Vector2>(startData, count, [this, startPos](Vector2& pt) {
		Vector2 pos;
		return !mTerrain->FindObstacle(startPos, pt, pos, 1);
	});
	_warnif((retIdx == count - 1));
	int nextIdx = (retIdx == count - 1) ? retIdx : retIdx + 1;
	Vector2 findPos = inPoints[nextIdx];
	outPoints.push_back(findPos);
	OptimizeRouteResults(inPoints, nextIdx, findPos, outPoints);
}