#include "jCrash.h"
#include "jMatrixControl.h"
#include "jGameObjectMgr.h"

jCrash::jCrash()
{
}


jCrash::~jCrash()
{
	if (mShape)
		delete mShape;
}

void jCrash::OnLoad()
{
	mCrashGrid = GetGameObject()->GetEngine().GetCrashGrid();
}
void jCrash::OnAddToGrid()
{
	jRect3D rect = GetRect();
	jRect rect2D = rect.TopBottom().ClipMinus();
	vector<list<jCrash*>*> grids;
	mCrashGrid->GetGrids(rect2D, grids);
	for (list<jCrash*>* grid : grids)
		grid->push_back(this);
}
void jCrash::OnUpdate()
{
	jRect3D rect = GetRect();
	jRect rect2D = rect.TopBottom().ClipMinus();
	vector<list<jCrash*>*> grids;
	mCrashGrid->GetGrids(rect2D, grids);
	for (list<jCrash*>* grid : grids)
	{
		list<jCrash*>::iterator me;
		for (auto iter = grid->begin(); iter != grid->end(); ++iter)
		{
			jCrash* candidate = *iter;
			if (candidate == this)
			{
				me = iter;
				continue;
			}

			CrashInfo result = CheckCrashed(this, candidate);
			if (result.crashed)
			{
				result.target = candidate;
				AddCrashedResult(result);
				result.target = this;
				candidate->AddCrashedResult(result);
			}
		}
		grid->erase(me);
	}

	CallbackCrashedObjects();
}


bool jCrash::IsCrash(jLine3D line)
{
	Vector3 center = mShape->GetCenter();
	Vector3 pt = line.ClosePoint(center);
	double dist = center.distance(pt);
	return dist < mShape->round;
}
jRect3D jCrash::GetRect()
{
	Vector3 parentPos = GetGameObject()->GetTransport().getPos();
	return mShape->GetRect(parentPos);
}
CrashInfo jCrash::CheckCrashed(jCrash * left, jCrash * right)
{
	return CrashInfo();
}
void jCrash::AddCrashedResult(CrashInfo result)
{
	jCrash* target = result.target;
	if (mCrashedList.find(target) != mCrashedList.end())
	{
		mCrashedList[target].isNew = false;
		mCrashedList[target].isKeep = true;
		mCrashedList[target].info = result;
	}
	else
	{
		CrashTrigInfo info;
		info.isNew = true;
		info.isKeep = false;
		info.info = result;
		mCrashedList[target] = info;
	}
}
void jCrash::CallbackCrashedObjects()
{
	jCrashInfos crashesEnter;
	jCrashInfos crashesKeep;
	jCrashInfos crashesLeave;
	for (auto iter = mCrashedList.begin(); iter != mCrashedList.end(); )
	{
		if (iter->second.isNew)
		{
			iter->second.isNew = false;
			iter->second.isKeep = false;
			crashesEnter->push_back(iter->second.info);
			++iter;
		}
		else if (iter->second.isKeep)
		{
			iter->second.isKeep = false;
			crashesKeep->push_back(iter->second.info);
			++iter;
		}
		else
		{
			crashesLeave->push_back(iter->second.info);
			mCrashedList.erase(iter++);
		}
	}
	if (mCallbacksEnter) mCallbacksEnter(crashesEnter);
	if (mCallbacksKeep) mCallbacksKeep(crashesKeep);
	if (mCallbacksLeave) mCallbacksLeave(crashesLeave);
}
