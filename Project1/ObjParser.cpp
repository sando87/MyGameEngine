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
		jShaderParserOnly(jParserD3 * parser) { mParser = parser; }
		virtual bool OnRender(ObjCamera* cam)
		{
			mParser->Render();
			return true;
		}
	};

	//mShader = new jShaderParser();
	//((jShaderParser*)mShader)->Load(mParser);
	//mShader->SetEnable(false);
	AddComponent(new jShaderParserOnly(mParser));

	Vector3 pos = GetTransform().getPos();
	pos.x = mOff;
	//GetTransform().moveTo(pos);

	if (mParser->mAnimKeys.size() > 0)
		AddComponent(new jEventParser());
}

void jEventParser::OnKeyDown(char ch)
{
	ObjParser* obj = (ObjParser*)GetGameObject();
	if (ch == 'Z')
	{
		mAnimIndex++;
		mAnimIndex %= obj->mParser->mAnimKeys.size();
		obj->mParser->mCurrentAnimIndex = mAnimIndex;
		_echoN(mAnimIndex);
	}
	else if (ch == 'X')
	{
		mAnimIndex--;
		mAnimIndex = mAnimIndex < 0 ? obj->mParser->mAnimKeys.size() - 1 : mAnimIndex;
		obj->mParser->mCurrentAnimIndex = mAnimIndex;
		_echoN(mAnimIndex);
	}
}

void jEventParser::OnKeyPressed(char ch)
{
	ObjParser* obj = (ObjParser*)GetGameObject();
	if (ch == 'C')
	{
		mAnimIndex++;
		mAnimIndex %= obj->mParser->mAnimKeys.size();
		obj->mParser->mCurrentAnimIndex = mAnimIndex;
	}
}
