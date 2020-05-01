#include "jCrash.h"
#include "jTransform.h"
#include "jGameObjectMgr.h"

#define CrashGridStep 8

unordered_multimap<u64, jCrash*> jCrash::mCrashGrid;

jCrash::jCrash()
{
}


jCrash::~jCrash()
{
}

void jCrash::OnLoad()
{
}
void jCrash::OnUpdate()
{
	jRect3D rect = GetRect();
	//jRect rect2D = rect.TopBottom().ClipMinus();
	vector<pair<u64,jCrashes>> grids;
	//GetGrids(rect2D, grids);
	for (auto grid : grids)
	{
		u64 key = grid.first;
		jCrashes crashes = grid.second;
		for (auto iter = crashes->begin(); iter != crashes->end(); ++iter)
		{
			jCrash* candidate = *iter;
			if (!candidate->GetEnable())
				continue;

			CrashInfo result = CheckCrashed(this, candidate);
			if (result.crashed)
			{
				result.target = candidate;
				OnCollision(result);
				result.target = this;
				candidate->OnCollision(result);
			}
		}
		mCrashGrid.insert(make_pair(key, this));
	}

}


bool jCrash::IsCrash(jLine3D line)
{
	Vector3 center = GetCenter();
	Vector3 pt = line.ClosePoint(center);
	double dist = center.distance(pt);
	return dist < GetRound();
}
jRect3D jCrash::GetRect()
{
	Vector3 parentPos = GetGameObject()->GetTransform().getPos();
	jRect3D rect = mShape.GetRect();
	rect.Move(parentPos);
	return rect;
}
Vector3 jCrash::GetCenter()
{
	Vector3 parentPos = GetGameObject()->GetTransform().getPos();
	Vector3 center = mShape.GetCenter();
	return center + parentPos;
}
void jCrash::Clear()
{
	mCrashGrid.clear();
}
CrashInfo jCrash::CheckCrashed(jCrash * left, jCrash * right)
{
	CrashInfo ret;
	Vector2 lPos = left->GetCenter();
	Vector2 rPos = right->GetCenter();
	double dist = lPos.distance(rPos);
	ret.crashed = dist < left->GetRound() + right->GetRound();
	ret.dist = dist;
	return ret;
}

//void jCrash::GetGrids(jRect rect, vector<pair<u64, jCrashes>>& grids)
//{
//	int leftIdx = rect.Left() / CrashGridStep;
//	int rightIdx = rect.Right() / CrashGridStep;
//	int btnIdx = rect.Bottom() / CrashGridStep;
//	int topIdx = rect.Top() / CrashGridStep;
//	for(int y = btnIdx; y <= topIdx; ++y)
//		for (int x = leftIdx; x <= rightIdx; ++x)
//		{
//			jCrashes crashes;
//			u64 key = ToU64(x, y);
//			auto range = mCrashGrid.equal_range(key);
//			for (auto iter = range.first; iter != range.second; ++iter)
//				crashes->push_back(iter->second);
//
//			grids.push_back(make_pair(key, crashes));
//		}
//}
