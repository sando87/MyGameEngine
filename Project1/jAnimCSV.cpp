#include "jAnimCSV.h"



jAnimCSV::jAnimCSV(string name)
{
	if (name.length() > 0)
		Load(name);
}


jAnimCSV::~jAnimCSV()
{
}

bool jAnimCSV::Load(string _filename)
{
	strings lines = jUtils::LoadLines(_filename);
	if (!lines)
		return false;

	int num = lines->size();
	int idx = 0;
	while(idx < num)
	{
		strings pieces = jUtils::Split2(lines[idx], ",");
		string clipName = pieces[0];
		mAnims[clipName] = AnimCSVInfo();
		AnimCSVInfo& clip = mAnims[clipName];
		clip.name = clipName;
		clip.frameRate = 1 / 30.0f;

		idx++;
		while (idx < num)
		{
			mat4s key;
			if (jUtils::CsvToMat(lines[idx], *key) == false)
				break;

			clip.keyMats.push_back(key);
			idx++;
		}

		clip.endTime = clip.frameRate * clip.keyMats.size();
	}

	return true;
}

mat4s jAnimCSV::Animate(float _deltaTime)
{
	mCurrentTime += _deltaTime;
	mat4s mats = mCurrentAnim->Animate(mCurrentTime);
	if (mCurrentTime > mCurrentAnim->endTime && mReturnAnim != nullptr)
		SetAnimation(mReturnAnim->name);

	return mats;
}

void jAnimCSV::SetAnimation(string name)
{
	mCurrentTime = 0;
	mCurrentAnim = &mAnims[name];
	mReturnAnim = nullptr;
}

void jAnimCSV::AddEvent(string name, float rate, function<void(void)> event)
{
	_warnif(mAnims.find(name) == mAnims.end());
	mAnims[name].AddEvent(rate, event);
}

void jAnimCSV::SetAnimation(string ToName, string BackName)
{
	mCurrentTime = 0;
	mCurrentAnim = &mAnims[ToName];
	mReturnAnim = &mAnims[BackName];
}
