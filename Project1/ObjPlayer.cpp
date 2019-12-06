#include "ObjPlayer.h"
#include "jInput.h"
#include "jModel.h"
#include "jShaderSkinned.h"
#include "jAnimate.h"
#include "jTexture.h"
#include "jLog.h"
#include "jLine3D.h"
#include "Vectors.h"
#include "Matrices.h"
#include "ObjCamera.h"
#include "ObjTerrainMgr.h"
#include "jGameObjectMgr.h"
#include "jTime.h"

ObjPlayer::ObjPlayer()
{
}


ObjPlayer::~ObjPlayer()
{
}

void ObjPlayer::OnStart()
{
	mTransport.init();
	mModel = new jModel();
	mModel->Load("mob.DAE");

	mShader = new jShaderSkinned();
	mShader->Initialize("./test.vs", "./test.ps");

	mTexture = new jTexture();
	mTexture->Initialize("./stone01.tga");

	mAnim = new jAnimate();
	mAnim->Load("mob.DAE");

	jInput::GetInst().mMouseDown += [&](jInput::jMouseInfo info)
	{
		Vector2 screenPt;
		Vector3 view = GetCamera().ScreenToWorldView(screenPt.x, screenPt.y);
		Vector3 pt = GetTerrain().CalcGroundPos(GetCamera().GetPosture().getPos(), view);
		pt = Vector3(5, 5, 0);
		mDestPoint = pt;

		jGameObjectMgr::GetInst().StartCoroutine([this, pt]() {
			float speed = 1; //1초(60프레임)당 움직이는 속도
			float speedRot = 30; //1초(60프레임)당 회전하는 속도
			float delta = jTime::Delta();
			Vector3 target = pt;
			target.z = mTransport.getPos().z;
			mTransport.rotateToPos_OnGround(target, speedRot * delta);
			mTransport.goForward(speed * delta);
			if (mTransport.getPos().distance(target) < 1)
				return false;
			return true;
		}, "MovePlayer");
	};
}

void ObjPlayer::OnUpdate()
{
}

void ObjPlayer::OnDraw()
{
	vector<Matrix4> mats;
	mAnim->Animate(mAnimTime, mats);

	mShader->SetParams(mModel, mTransport.getMatrix(), &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1, 1, -1, 0), mats);
	mShader->Render();
}
