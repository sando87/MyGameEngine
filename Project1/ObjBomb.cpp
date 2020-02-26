#include "ObjBomb.h"
#include "ObjEffect.h"
#include "jTransform.h"
#include "jTime.h"
#include "jGameObjectMgr.h"

ObjBomb::ObjBomb()
{
}

ObjBomb::~ObjBomb()
{
}

void ObjBomb::OnLoad()
{
	LoadTxt("MyObject_51.txt");
	mDest = Vector3(-30, 30, 0);
	mSecond = 5;
	mAccTime = 0;
}

void ObjBomb::OnStart()
{
	mEffectFire = new ObjEffect();
	mEffectFire->SetImgFullname("./res/img/98_0000028E35C11160_tt.tga");
	mEffectFire->SetStepUV(Vector2f(0.125f, 1.0f));
	jGameObjectMgr::GetInst().AddGameObject(mEffectFire);

	mEffectSmoke = new ObjEffect();
	mEffectSmoke->SetImgFullname("./res/img/97_0000028E8BECD460_t.tga");
	mEffectSmoke->SetStepUV(Vector2f(0.25f, 1.0f));
	jGameObjectMgr::GetInst().AddGameObject(mEffectSmoke);

	Vector3 myPos = GetTransform().getPos();
	mDirection = mDest - myPos;
	mDistance = mDirection.length();
	mDirection.normalize();

	mHeights.Init(Vector2(mSecond / 2, 30), Vector2(0, 0));
}

void ObjBomb::OnUpdate()
{
	jTransform& trans = GetTransform();

	mAccTime += jTime::Delta();
	if (mAccTime > 0.3)
	{
		mAccTime = 0;
		mEffectFire->Burst(trans.getPos(), 0, 0);
		mEffectSmoke->Burst(trans.getPos(), 1, 0);
	}
	
	double height = mHeights.CalcYAcc(jTime::Delta());
	double deltaDist = mDistance * jTime::Delta() / mSecond;
	Vector2 newPos = trans.getPos() + (mDirection * deltaDist);
	trans.moveTo(Vector3(newPos.x, newPos.y, height));
	trans.rotateAxis(mDirection, 180 * jTime::Delta());
}
