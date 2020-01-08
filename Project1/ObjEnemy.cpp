#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimCSV.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "ObjTerrainMgr.h"
#include "jInput.h"
#include "jTime.h"
#include "jCrash.h"

ObjEnemy::ObjEnemy()
{
}


ObjEnemy::~ObjEnemy()
{
}

void ObjEnemy::OnStart()
{
	LoadTxt("MyObject_232.txt");
	mAnim = FindComponent<jAnimCSV>();
	mShader = FindComponent<jShaderSkin>();

	ShaderParamsSkin& param = mShader->GetParams();
	param.material.diffuse = Vector4f(1, 1, 1, 1);
	param.light.direction = Vector4f(-1, -1, -1, 0);
	AddComponent((new jCrash())->Init(1, 2, [](jCrashs objs) {}) );

	mAnim->SetAnimation("idle");
	StartCoRoutine("enemyTimer", 5000, [this]() {  //start timer every 5sec
		int cmd = jUtils::Random() % 4;
		if (cmd == 1)
		{
			Vector3 pos = GetTransport().getPos();
			pos.x += jUtils::Random() % 50 - 25;
			pos.y += jUtils::Random() % 50 - 25;
			WalkTo(pos, 10);
		}
		else if (cmd == 2)
		{
			mAnim->SetAnimation("attack", "idle");
			StopCoRoutine("CoroutineWalk");
		}

		return CoroutineReturn::Keep;
	});
}

void ObjEnemy::OnUpdate()
{
	mat4s mats = mAnim->Animate(jTime::Delta());
	ShaderParamsSkin& param = mShader->GetParams();
	for (int i = 0; i < 45; ++i)
		param.bones[i] = mats[i];
}


bool ObjEnemy::WalkTo(Vector3 target, float speed)
{
	mAnim->SetAnimation("walk");

	GetTransport().lookPos(target);
	StartCoRoutine("CoroutineWalk", [target, speed, this]() {
		float dist = GetTransport().getPos().distance(target);
		if (dist < 1)
		{
			mAnim->SetAnimation("idle");
			return CoroutineReturn::Stop;
		}

		GetTransport().goForward(jTime::Delta() * speed);
		return CoroutineReturn::Keep;
	});

	return true;
}
