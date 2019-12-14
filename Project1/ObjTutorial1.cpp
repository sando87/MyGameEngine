#include "ObjTutorial1.h"
#include "jParserD3.h"
#include "jParserS2.h"
#include "jShaderParser.h"
#include "jMatrixControl.h"

#define IS_DIABLO

ObjTutorial1::ObjTutorial1()
{
	mFileIndex = 0;
	mOff = 0;
#ifdef IS_DIABLO
	mParser = new jParserD3();
#else
	mParser = new jParserS2();
#endif
}

ObjTutorial1::~ObjTutorial1()
{
	delete mParser;
	delete mShader;
}

void ObjTutorial1::OnStart()
{
	if (!mParser->Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		DeleteFromMgr();
		return;
	}

	mShader = new jShaderParser();
	mShader->Load(mParser);
	AddComponent(mShader);

	//mParser->ExportToObjectFormat();

	Vector3 pos = mTransport->getPos();
	pos.x = mOff;
	mTransport->moveTo(pos);
}

void ObjTutorial1::OnUpdate()
{
}

void ObjTutorial1::OnDraw()
{
	mShader->Render();
}