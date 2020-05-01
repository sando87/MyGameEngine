#include "jAnimator.h"
#include "jShaderSkin.h"
#include "jTime.h"
#include "jCaches.h"
#include "jHealthPoint.h"

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
	//SplitCSVtoClips();

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
		mHP = GetGameObject()->FindComponent<jHealthPoint>();
		_exceptif(nullptr == mHP || nullptr == mShader, return);
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
	_exceptif (nullptr == mCurrentAnim, return mat4s());
	float speed = GetAnimationSpeed();
	float endTime = mCurrentAnim->endTime * speed;
	float preTime = mPreTimeRate * endTime;
	float curTime = preTime + _deltaTime;
	float curRate = curTime / endTime;
	curRate = curRate - (int)curRate;
	int matIdx = curRate * mCurrentAnim->keyMats.size();

	//InvokeEvent
	float fixedRate = curRate < mPreTimeRate ? curRate + 1.0f : curRate;
	for (auto item : mCurrentAnim->events)
	{
		float eventRate = item.first;
		if (mPreTimeRate < eventRate && eventRate < fixedRate)
			item.second();
	}

	mPreTimeRate = curRate;
	return mCurrentAnim->keyMats[matIdx];
}
float jAnimator::GetAnimationSpeed()
{
	if (mHP == nullptr || mHP->CurSpec.MoveSpeed == 0)
		return 1.0;

	if (mCurrentAnim->name == "walk")
	{
		return mCurrentAnim->moveSpeed / mHP->CurSpec.MoveSpeed;
	}
	//else if(mCurrentAnim->name == "attack1")
	//{
	//	float periodAttack = 0.7666f; //mHP->CurSpec.attackSpeed; 1회 공격당 걸리는 시간(주기)
	//	return periodAttack / mCurrentAnim->endTime;
	//}
	return 1.0;
}
void jAnimator::SetEvent(string animName, float rate, function<void(void)> event)
{
	_exceptif(mAnims.find(animName) == mAnims.end(), return);
	if (mAnims.find(animName) != mAnims.end())
		mAnims[animName].events[rate] = event;
}

void jAnimator::ClearEvent(string animName)
{
	_exceptif(mAnims.find(animName) == mAnims.end(), return);
	if (mAnims.find(animName) != mAnims.end())
		mAnims[animName].events.clear();
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
		float frameRate = 1 / 30.0f;

		idx++;
		while (idx < num)
		{
			mat4s key;
			if (jUtils::CsvToMat(lines[idx], *key) == false)
				break;

			clip.keyMats.push_back(key);
			idx++;
		}

		clip.endTime = frameRate * clip.keyMats.size();
		clip.moveSpeed = 4.0f;
		clip.SaveToFile(baseFullname + "_" + clipName + ".dump");
	}
}

void jAnimator::SetAnimation(string name)
{
	_warnif(mAnims.find(name) == mAnims.end());
	if (mAnims.find(name) != mAnims.end())
	{
		mCurrentAnim = &mAnims[name];
		mPreTimeRate = 0;
	}
}

string jAnimator::GetAnimation()
{
	return mCurrentAnim == nullptr ? "" : mCurrentAnim->name;
}

void AnimationClipOri::SaveToFile(string fullname)
{
	AnimFileHead head;
	head.magic = 0x7a;
	sprintf_s(head.name, "%s", name.c_str());
	head.frameRate = 1 / 30.0f;
	head.endTime = endTime;
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

bool AnimationClipOri::LoadFromFile(string fullname)
{
	chars buf = jUtils::LoadFile2(fullname);
	if (!buf)
		return false;

	AnimFileHead * head = (AnimFileHead *)&buf[0];
	name = head->name;
	endTime = head->endTime;
	moveSpeed = 4.0f;
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

void AnimationClip::SaveToFile(string fullname)
{
	magic = 0x7a;
	headerLen = 128;
	int bodyLen = sizeof(Matrix4) * boneCount * keyCount;
	string headerString = HeaderToString();
	_exceptif(headerString.length() > 120, return);

	int totalSize = headerLen + bodyLen;
	char * buf = (char *)malloc(totalSize);
	memset(buf, 0x00, totalSize);
	memcpy(buf, &magic, 4);
	memcpy(buf + 4, &headerLen, 4);
	memcpy(buf + 8, headerString.c_str(), headerString.length());
	char * data = buf + headerLen;
	for (int i = 0; i < keyMats.size(); ++i)
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
	_exceptif(!buf, return false);

	char* pBuf = &buf[0];
	magic = *((int*)pBuf);
	headerLen = *((int*)(pBuf + 4));
	string headerString = string(pBuf + 8);
	StringToHead(headerString);

	char* curKey = &buf[0] + headerLen;
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
