#include "ObjParser.h"
#include "jParserD3.h"
#include "jParserS2.h"
#include "jShaderParser.h"
#include "jMatrixControl.h"

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
	AddComponent(mShader);

	//mParser->ExportToObjectFormat();

	Vector3 pos = mTransport->getPos();
	pos.x = mOff;
	mTransport->moveTo(pos);
}

