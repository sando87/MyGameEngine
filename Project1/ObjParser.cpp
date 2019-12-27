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

	//for (int i = 0; i < mParser->anims[0].keys.size(); ++i)
	//{
	//	Matrix4f& mat = mParser->anims[0].keys[i][30];
	//	string matStr = jUtils::MatToCSV(&mat);
	//	jUtils::SaveToFile(".", "animTest.csv", matStr, true);
	//}
	

	mShader = new jShaderParser();
	mShader->Load(mParser);
	AddComponent(mShader);

	mParser->ExportToObjectFormat();

	Vector3 pos = mTransport->getPos();
	pos.x = mOff;
	mTransport->moveTo(pos);

	//if (mParser->anims.size() > 0)
	//{
	//	jInput::GetInst().mKeyboard += [this](const unsigned char* info)
	//	{
	//		_echoN(animIndex);
	//		PrintMat(&mParser->anims[0].keys[animIndex][0]);
	//		for (int i = 0; i < 45; ++i)
	//			mShader->mBones[i] = mParser->anims[0].keys[animIndex][i];
	//
	//		animIndex++;
	//		animIndex %= mParser->anims[0].keys.size();
	//
	//	};
	//}
}

