#include "ObjParser.h"
#include "jParserD3.h"
#include "jParserS2.h"
#include "jShaderParser.h"
#include "jMatrixControl.h"
#include "jInput.h"

#define IS_DIABLO

ObjParser::ObjParser()
{
	mFileIndex = 0;
	mOff = 0;
#ifdef IS_DIABLO
	mParser = new jParserD3();
#else
	mParser = new jParserS2();
#endif
}

ObjParser::~ObjParser()
{
	delete mParser;
	delete mShader;
}

void PrintMat(Matrix4f* mat)
{
	const float* p = mat->get();
	_printlog("[%f, %f, %f, %f]\n", p[0], p[1], p[2], p[3]);
	_printlog("[%f, %f, %f, %f]\n", p[4], p[5], p[6], p[7]);
	_printlog("[%f, %f, %f, %f]\n", p[8], p[9], p[10], p[11]);
	_printlog("[%f, %f, %f, %f]\n", p[12], p[13], p[14], p[15]);
}
void ObjParser::OnStart()
{
	if (!mParser->Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	mShader = new jShaderParser();
	mShader->Load(mParser);
	mShader->SetVisiable(false);
	AddComponent(mShader);

	Vector3f basePos;
	basePos.x = mParser->mCBMain.matWorld[3];
	basePos.y = mParser->mCBMain.matWorld[7];
	basePos.z = mParser->mCBMain.matWorld[11];
	//_printlog("%f %f %f \n", basePos.x, basePos.y, basePos.z);
	Vector3 pos = mTransport->getPos();
	//pos.x = mOff;
	pos = basePos;
	mTransport->moveTo(pos);

	if (mParser->mAnims.size() > 0)
	{
		jInput::GetInst().mKeyboard += [this](const unsigned char* key)
		{
			_echoN(animIndex);
			for (int i = 0; i < 45; ++i)
				mShader->mBones[i] = mParser->mAnims[0].keys[animIndex][i];


			if (key[30] != 0)
			{
				animIndex++;
				animIndex %= mParser->mAnims[0].keys.size();
				Sleep(500);
			}
			else if (key[31] != 0)
			{
				animIndex--;
				animIndex = animIndex < 0 ? mParser->mAnims[0].keys.size() - 1 : animIndex;
				Sleep(500);
			}
			else
			{
				animIndex++;
				animIndex %= mParser->mAnims[0].keys.size();
			}

	
		};
	}
}

