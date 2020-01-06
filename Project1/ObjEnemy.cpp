#include "ObjEnemy.h"
#include "ObjCamera.h"
#include "jAnimCSV.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "ObjTerrainMgr.h"
#include "jInput.h"
#include "jTime.h"

ObjEnemy::ObjEnemy()
{
}


ObjEnemy::~ObjEnemy()
{
}

void ObjEnemy::OnStart()
{
	LoadTxt("MyObject_397.txt");
	mAnim = FindComponent<jAnimCSV>();
	mShader = FindComponent<jShaderSkin>();

	ShaderParamsSkin& param = mShader->GetParams();
	for (int i = 0; i < 45; ++i)
		param.bones[i] = Matrix4().identity();
	param.material.diffuse = Vector4f(1, 1, 1, 1);
	param.light.direction = Vector4f(-1, -1, -1, 0);

	mAnim->SetAnimation("idle");
	StartCoRoutine([this]() {
		int cmd = jUtils::Random() % 4;
		if (cmd == 1)
		{
			Vector3 pos = GetTransport().getPos();
			pos.x += jUtils::Random() % 50 - 25;
			pos.y += jUtils::Random() % 50 - 25;
			WalkTo(pos);
		}
		else if (cmd == 2)
		{
			mAnim->SetAnimation("attack", "idle");
			StopCoRoutine("CoroutineWalk");
		}
		else if (cmd == 3)
		{
			mAnim->SetAnimation("attack2", "idle");
			StopCoRoutine("CoroutineWalk");
		}

		return CoroutineReturn::Keep;
	}, 5000, "enemyTimer");
}

void ObjEnemy::OnUpdate()
{
	mat4s mats = mAnim->Animate(jTime::Delta());
	ShaderParamsSkin& param = mShader->GetParams();
	for (int i = 0; i < 45; ++i)
		param.bones[i] = mats[i];
}


bool ObjEnemy::WalkTo(Vector3 target)
{
	mAnim->SetAnimation("walk");

	GetTransport().lookPos(target);
	StartCoRoutine([target, this]() {
		float dist = GetTransport().getPos().distance(target);
		if (dist < 1)
		{
			mAnim->SetAnimation("idle");
			return CoroutineReturn::Stop;
		}

		GetTransport().goForward(jTime::Delta() * 10);
		return CoroutineReturn::Keep;
	}, "CoroutineWalk");

	return true;
}
