#include "jAnimator.h"
#include "jShaderSkin.h"
#include "jTime.h"
#include "jCaches.h"

struct AnimFileHead
{
	int magic;
	char name[64];
	float frameRate;
	float endTime;
	float prevPosRate;
	int boneCount;
	int keyCount;
	Matrix4* mats[];
};

jAnimator::jAnimator(string name)
{
	SetFullname(name);
}
jAnimator::~jAnimator()
{
}
void jAnimator::OnLoad()
{
	SplitCSVtoClips();

	string baseFullname = jUtils::RemoveExtension(GetFullname());
	string filter = baseFullname + "_*.dump";
	jUtils::ForEachFiles2(nullptr, filter.c_str(), [&](void* obj, string filename) {
		string fullname = string(PATH_RESOURCES) + "anim/" + filename;
		AnimationClip * retClip = (AnimationClip *)jCaches::CacheClass(fullname, [](string _fullname) {
			AnimationClip * clip = new AnimationClip();
			clip->LoadFromFile(_fullname);
			return clip;
		});

		if (retClip != nullptr)
			mAnims[retClip->name] = *retClip;
		return true;
	});

	if (!mAnims.empty())
	{
		_errorif(mAnims.find("idle") == mAnims.end());
		SetAnimation("idle");
		mShader = GetGameObject()->FindComponent<jShaderSkin>();
	}
}
void jAnimator::OnUpdate()
{
	if (mShader != nullptr)
	{
		mat4s mats = Animate(jTime::Delta());
		ShaderBufferSkin& param = mShader->GetParamSkin();
		for (int i = 0; i < 45; ++i)
		{
			param.bones[i] = mats[i];
		}
	}
}
mat4s jAnimator::Animate(float _deltaTime)
{
	mCurrentTime += _deltaTime;
	mat4s mats = mCurrentAnim->Animate(mCurrentTime);
	return mats;
}
void jAnimator::AddEvent(string animName, float rate, function<void(void)> event)
{
	_warnif(mAnims.find(animName) == mAnims.end());
	if (mAnims.find(animName) != mAnims.end())
		mAnims[animName].AddEvent(rate, event);
}

void jAnimator::SplitCSVtoClips()
{
	string fullname = GetFullname();
	if (fullname.length() == 0)
		return;

	string baseFullname = jUtils::RemoveExtension(fullname);
	string findName = jUtils::FindFile(baseFullname + "_*.dump");
	if (findName.length() != 0)
		return;

	strings lines = jUtils::LoadLines(fullname);
	if (!lines)
		return;

	int num = lines->size();
	int idx = 0;
	while (idx < num)
	{
		AnimationClip clip;
		strings pieces = jUtils::Split2(lines[idx], ",");
		string clipName = pieces[0];
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
		clip.SaveToFile(baseFullname + "_" + clipName + ".dump");
	}
}

void jAnimator::SetAnimation(string name)
{
	_warnif(mAnims.find(name) == mAnims.end());
	if (mAnims.find(name) != mAnims.end())
	{
		mCurrentAnim = &mAnims[name];
		mCurrentAnim->prevPosRate = 0;
		mCurrentTime = 0;
	}
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
void AnimationClip::SaveToFile(string fullname)
{
	AnimFileHead head;
	head.magic = 0x7a;
	sprintf_s(head.name, "%s", name.c_str());
	head.frameRate = frameRate;
	head.endTime = endTime;
	head.prevPosRate = prevPosRate;
	head.boneCount = keyMats.empty() ? 0 : keyMats.front()->size();
	head.keyCount = keyMats.size();

	int totalSize = sizeof(head) + (sizeof(Matrix4) * head.boneCount * head.keyCount);
	char * buf = (char *)malloc(totalSize);
	memcpy(buf, &head, sizeof(head));
	char * data = buf + sizeof(head);
	for (int i = 0; i < head.keyCount; ++i)
	{
		mat4s mats = keyMats[i];
		int size = sizeof(Matrix4) * mats->size();
		memcpy(data, &mats[0], size);
		data += size;
	}

	jUtils::SaveToFile(fullname, buf, totalSize);
	free(buf);
}

bool AnimationClip::LoadFromFile(string fullname)
{
	chars buf = jUtils::LoadFile2(fullname);
	if (!buf)
		return false;

	AnimFileHead * head = (AnimFileHead *)&buf[0];
	name = head->name;
	frameRate = head->frameRate;
	endTime = head->endTime;
	prevPosRate = head->prevPosRate;
	int boneCount = head->boneCount;
	int keyCount = head->keyCount;

	char* curKey = &buf[0] + sizeof(AnimFileHead);
	int size = sizeof(Matrix4) * boneCount;
	for (int i = 0; i < keyCount; ++i)
	{
		mat4s keyframe;
		keyframe->resize(boneCount);
		memcpy(&keyframe[0], curKey, size);
		keyMats.push_back(keyframe);
		curKey += size;
	}
	return true;
}
