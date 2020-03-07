#include "ObjParser.h"
#include "jParserD3.h"
#include "jParserS2.h"
#include "jShaderParser.h"
#include "jTransform.h"
#include "jInputEvent.h"

class jEventParser : public jInputEvent
{
private:
	int mAnimIndex = 0;
	virtual void OnKeyDown(char ch);
	virtual void OnKeyPressed(char ch);
};

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
		Destroy();
		return;
	}


	class jShaderParserOnly : public jShader
	{
	public:
		jParserD3 * mParser;
		virtual bool OnRender(ObjCamera* cam)
		{
			mParser->Render();
			return true;
		}
	};

	mShader = new jShaderParserOnly();
	((jShaderParserOnly*)mShader)->mParser = mParser;
	//mShader->Load(mParser);
	//mShader->SetEnable(false);
	AddComponent(mShader);

	Vector3f basePos;
	//basePos.x = mParser->mCBMain.matWorld[3];
	//basePos.y = mParser->mCBMain.matWorld[7];
	//basePos.z = mParser->mCBMain.matWorld[11];
	//_printlog("%f %f %f \n", basePos.x, basePos.y, basePos.z);
	Vector3 pos = GetTransform().getPos();
	pos.x = mOff;
	//pos = basePos;
	GetTransform().moveTo(pos);

	//if (mParser->mAnims.size() > 0)
	//	AddComponent(new jEventParser());
}

void jEventParser::OnKeyDown(char ch)
{
	ObjParser* obj = (ObjParser*)GetGameObject();
	if (ch == 'Z')
	{
		mAnimIndex++;
		mAnimIndex %= obj->mParser->mAnims[0].keys.size();
		_echoN(mAnimIndex);
		for (int i = 0; i < 45; ++i)
			((jShaderParser *)obj->mShader)->mBones[i] = obj->mParser->mAnims[0].keys[mAnimIndex][i];
	}
	else if (ch == 'X')
	{
		mAnimIndex--;
		mAnimIndex = mAnimIndex < 0 ? obj->mParser->mAnims[0].keys.size() - 1 : mAnimIndex;
		_echoN(mAnimIndex);
		for (int i = 0; i < 45; ++i)
			((jShaderParser *)obj->mShader)->mBones[i] = obj->mParser->mAnims[0].keys[mAnimIndex][i];
	}
}

void jEventParser::OnKeyPressed(char ch)
{
	ObjParser* obj = (ObjParser*)GetGameObject();
	if (ch == 'C')
	{
		mAnimIndex++;
		mAnimIndex %= obj->mParser->mAnims[0].keys.size();
		for (int i = 0; i < 45; ++i)
			((jShaderParser *)obj->mShader)->mBones[i] = obj->mParser->mAnims[0].keys[mAnimIndex][i];
	}
}
