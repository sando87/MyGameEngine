#include <functional>

#include "ObjTutorial1.h"
#include "jModel.h"
#include "jLog.h"
#include "jInput.h"
#include "jSound.h"
#include "jTexture.h"
#include "jBoneTree.h"
#include "ObjCamera.h"
#include "jShaderSprite.h"
#include "jShaderSkinned.h"
#include "jShaderColor.h"
#include "jLine3D.h"

ObjTutorial1::ObjTutorial1()
{
	mShader = nullptr;
	mModel = nullptr;
	mIdxUV = 0;
	mAnimTime = 0.0f;

}


ObjTutorial1::~ObjTutorial1()
{
	if (mShader != nullptr)
	{
		mShader->Release();
		delete mShader;
		mShader = nullptr;
	}
	if (mModel != nullptr)
	{
		mModel->Release();
		delete mModel;
		mModel = nullptr;

	}
}

void ObjTutorial1::OnStart()
{
	mWorldMat.identity();

	mModel = new jModel();
	mModel->Load("mob_new2.DAE");
	//mModel->LoadSprite(Vector3f(0, 0, 0), Vector2f(0.2f, 0.2f));

	mShader = new jShaderSkinned();
	mShader->Initialize("./test.vs", "./test.ps");

	mTexture = new jTexture();
	mTexture->Initialize("./stone01.tga");

	mAnim = new jBoneTree();
	mAnim->Load("mob_new2.DAE");

	//int size = mAnim->mVecBones.size();
	//vector<Vector3> vec;
	//for (int i = 0; i < size; ++i)
	//{
	//	Matrix4 mm = mAnim->mVecBones[i]->mMat;
	//	vec.push_back(Vector3f(mm[7], mm[3], mm[11]));
	//}
	//mModel->LoadPointList(vec, 0.01f);

	jInput::GetInst().mMouse += [&](auto info)
	{
		if (info.left & 0x80)
		{
			//static bool tt = true;
			//if (tt)
			{
				Vector3 view = GetCamera().GetViewOnMouse(480, 240);
				jLine3D line(GetCamera().GetPosture().getPos(), view);
				Position2 pt = line.GetXY(0);
				_echoF(pt.x);
				_echoF(pt.y);
				//GetCamera().GetPosture().lookat(view);
				//tt = false;
			}
		}
	};

	jInput::GetInst().mKeyboard = [&](auto keys)
	{
		//jSound::GetInst().PlayWaveFile();
		//mWorldMat[14] += 1.0;
		//GetCamera().goForward(0.1f);
		if (keys[DIK_A] & 0x80)
		{
			mIdxUV++;
			mAnimTime += 0.01f;
		}
	};
}

void ObjTutorial1::OnUpdate()
{
	
}

void ObjTutorial1::OnDraw()
{
	vector<Matrix4> mats;
	//mats.push_back(Matrix4().identity());
	mAnim->Animate(mAnimTime, mats);
	//for (int i = 0;i < cnt; ++i)
	//{
	//	Matrix4 mm = mAnim->mVecBones[i].mMatLocal;
	//	mm.invert();
	//	mm.transpose();
	//	mats.push_back(mm.transpose());//mAnim->mVecBones[i]->mMat);
	//	mats.push_back(mm);
	//}
	//mShader->SetParams(mModel, mWorldMat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector2f(0.2f, 0.2f), Vector2n(mIdxUV % 5, mIdxUV / 5));
	mShader->SetParams(mModel, mWorldMat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1,1,-1,0), mats);
	mShader->Render();
	
}