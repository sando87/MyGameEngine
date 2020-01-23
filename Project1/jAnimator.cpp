#include "jAnimator.h"
#include "jShaderSkin.h"
#include "jTime.h"

jAnimator::jAnimator(string name)
{
	SetFullname(name);
}
jAnimator::~jAnimator()
{
}
void jAnimator::OnLoad()
{
	strings lines = jUtils::LoadLines(GetFullname());
	if (!lines)
		return;

	int num = lines->size();
	int idx = 0;
	while(idx < num)
	{
		strings pieces = jUtils::Split2(lines[idx], ",");
		string clipName = pieces[0];
		if(mAnims.find(clipName) == mAnims.end())
			mAnims[clipName] = AnimationClip();
		AnimationClip& clip = mAnims[clipName];
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
	SetAnimation("idle");
	mShader = GetGameObject()->FindComponent<jShaderSkin>();
}
void jAnimator::OnUpdate()
{
	if (mShader != nullptr)
	{
		mat4s mats = Animate(jTime::Delta());
		ShaderParamsSkin& param = mShader->GetParams();
		for (int i = 0; i < 45; ++i)
			param.bones[i] = mats[i];
	}
}
mat4s jAnimator::Animate(float _deltaTime)
{
	mCurrentTime += _deltaTime;
	mat4s mats = mCurrentAnim->Animate(mCurrentTime);
	return mats;
}
void jAnimator::AddEvent(string name, float rate, function<void(void)> event)
{
	if (mAnims.find(name) == mAnims.end())
		mAnims[name] = AnimationClip();

	mAnims[name].AddEvent(rate, event);
}

void jAnimator::SetAnimation(string name)
{
	if (mAnims.find(name) == mAnims.end())
		mAnims[name] = AnimationClip();

	mCurrentTime = 0;
	mCurrentAnim = &mAnims[name];
}

string jAnimator::GetAnimation()
{
	return mCurrentAnim == nullptr ? "" : mCurrentAnim->name;
}


mat4s AnimationClip::Animate(float _time)
{
	float a = _time / endTime;
	float b = a - (int)a;
	float c = b * endTime;
	int n = (int)(c / frameRate);
	ProcEvent(b);
	return keyMats[n];
}
void AnimationClip::AddEvent(float rate, function<void(void)> event)
{
	events.push_back(make_pair(rate, event));
}
void AnimationClip::ProcEvent(float currentRate)
{
	float fixedRate = currentRate < prevPosRate ? currentRate + 1.0f : currentRate;
	for (auto item : events)
	{
		float eventRate = item.first;
		if (prevPosRate < eventRate && eventRate < fixedRate)
			item.second();
	}
	prevPosRate = currentRate;
}