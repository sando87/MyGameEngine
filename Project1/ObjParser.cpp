#include "ObjParser.h"
#include "jParserD3.h"
#include "jParserS2.h"
#include "jShaderParser.h"
#include "jTransform.h"
#include "cUserInputDriven.h"
#include "jAnimator.h"
#include "cCuiDriven.h"
#include "jTinyDB.h"

#define IS_DIABLO

ObjParser::ObjParser()
{
	mFileIndex = 0;
	mAnimIndex = 0;
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

void PrintMat(Matrix3x4f* mat)
{
	const float* p = mat->a;
	for (int i = 0; i < 12; ++i)
		_printlog("%.3f ", p[i]);
	_printlog("\n");
}
void ObjParser::OnLoad()
{
	if (!mParser->Init(mFileIndex))
	{
		_printlog("fail to load [%d]\n", mFileIndex);
		Destroy();
		return;
	}

	GetEngine().StartCoRoutine("test", 1000, [](CorMember& mem, bool isFirst) {
		_trace();
		return CorCmd::Keep;
	});

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

	//jShaderParser* shader = new jShaderParser();
	//shader->Load(mParser);
	//AddComponent(shader);
	
	AddComponent(new jShaderParserOnly(mParser));

	Vector3 pos = GetTransform().getPos();
	pos.x = mOff;
	//GetTransform().moveTo(pos);

	if (mParser->mAnimKeys.size() > 0)
	{
		cUserInputDriven* parserEvent = new cUserInputDriven();
		parserEvent->EventKeyDown = [&](InputEventArgs args) { 
			OnKeyDown(args.key);
			return EventResult::TransferEvent;
		};
		parserEvent->EventKeyPressed = [&](InputEventArgs args) { 
			OnKeyPressed(args.key);
			return EventResult::TransferEvent;
		};
		AddComponent(parserEvent);
	}

	cCuiDriven* cui = new cCuiDriven();
	cui->EventCommandInput = [&](string userInput){
		strings pieces = jUtils::Split2(userInput, " ");
		if (!pieces)
			return;

		string cmd = pieces[0];
		if (cmd == "clip")
		{
			if (pieces->size() != 7)
			{
				_printlog("clip 20 40 walk loop 3.2(distPerLoop) 90(rotateDeg)\n");
				return;
			}
			
			int sIdx = jUtils::ToInt(pieces[1]);
			int eIdx = jUtils::ToInt(pieces[2]);
			string name = pieces[3];
			bool isLoop = pieces[4] == "loop" ? true : false;
			string distPerLoop = pieces[5];
			float rotDeg = jUtils::ToDouble(pieces[6]);
			ClipAnimation(sIdx, eIdx, name, isLoop, distPerLoop, rotDeg);
		}
		else if (cmd == "matidx")
		{
			if (pieces->size() != 2)
			{
				_printlog("matidx 0(0~44)\n");
				return;
			}
			mPrintMatIdx = jUtils::ToInt(pieces[1]);
		}
		else if (cmd == "logidx")
		{
			if (pieces->size() != 2)
			{
				_printlog("logidx 0(fileIndex)\n");
				return;
			}
			mPrintLogIdx = jUtils::ToInt(pieces[1]);
		}
		else if (cmd == "exportitem")
		{
			if (pieces->size() != 3)
			{
				_printlog("exportitem 0(fileIndex) armor\n");
				return;
			}
			int fileIndex = jUtils::ToInt(pieces[1]);
			string category = pieces[2];
			if (fileIndex == mFileIndex)
				ExportItemResource(category);
		}
		else if (cmd == "exportskin")
		{
			if (pieces->size() != 2)
			{
				_printlog("exportskin 90(+z rotate degree)\n");
				return;
			}
			float rotDeg = jUtils::ToDouble(pieces[1]);
			mParser->ReadyToExportInfo("skin", false, true);
			mParser->mExportInfo.ExportImage(PATH_RESOURCES + string("img/"));
			mParser->mExportInfo.ExportMeshDump(PATH_RESOURCES + string("mesh/"), rotDeg);
			mParser->mExportInfo.ExportMetaInfo(PATH_RESOURCES + string("meta/"));
		}
		else
		{
			_printlog("undefined cmd [%s]\n", userInput.c_str());
		}

		_printlog("end [%s]\n", userInput.c_str());
	};
	AddComponent(cui);
}

void ObjParser::OnKeyDown(char ch)
{
	if (ch == 'Z')
	{
		mAnimIndex++;
		mAnimIndex %= mParser->mAnimKeys.size();
		mParser->mCurrentAnimIndex = mAnimIndex;
		if (mFileIndex == mPrintLogIdx)
		{
			_printlog("[%d][%d][%d] ", mPrintLogIdx, mAnimIndex, mPrintMatIdx);
			PrintMat(&mParser->mAnimKeys[mAnimIndex].bones[mPrintMatIdx]);
		}
	}
	else if (ch == 'X')
	{
		mAnimIndex--;
		mAnimIndex = mAnimIndex < 0 ? mParser->mAnimKeys.size() - 1 : mAnimIndex;
		mParser->mCurrentAnimIndex = mAnimIndex;
		if (mFileIndex == mPrintLogIdx)
		{
			_printlog("[%d][%d][%d] ", mPrintLogIdx, mAnimIndex, mPrintMatIdx);
			PrintMat(&mParser->mAnimKeys[mAnimIndex].bones[mPrintMatIdx]);
		}
	}
}

void ObjParser::OnKeyPressed(char ch)
{
	if (ch == 'C')
	{
		mAnimIndex++;
		mAnimIndex %= mParser->mAnimKeys.size();
		mParser->mCurrentAnimIndex = mAnimIndex;
		if (mFileIndex == mPrintLogIdx)
		{
			_printlog("[%d][%d][%d] ", mPrintLogIdx, mAnimIndex, mPrintMatIdx);
			PrintMat(&mParser->mAnimKeys[mAnimIndex].bones[mPrintMatIdx]);
		}
	}
	else if (ch == 'V')
	{
		mAnimIndex--;
		mAnimIndex = mAnimIndex < 0 ? mParser->mAnimKeys.size() - 1 : mAnimIndex;
		mParser->mCurrentAnimIndex = mAnimIndex;
		if (mFileIndex == mPrintLogIdx)
		{
			_printlog("[%d][%d][%d] ", mPrintLogIdx, mAnimIndex, mPrintMatIdx);
			PrintMat(&mParser->mAnimKeys[mAnimIndex].bones[mPrintMatIdx]);
		}
	}
}

void ObjParser::ClipAnimation(int sIdx, int eIdx, string name, bool isLoop, string distPerLoop, float rotDeg)
{
	if (mParser->mAnimFileName.length() == 0)
		return;

	string animName = jUtils::GetFilenameWithoutExt(mParser->mAnimFileName);
	AnimationClip metaInfo;
	metaInfo.name = name;
	metaInfo.animFilename = animName + "_" + name + ".dump";
	metaInfo.isLoop = isLoop;

	Matrix4 rotMat;
	rotMat.identity();
	rotMat.rotateZ(rotDeg);

	stringstream ss;
	ss << mParser->mContext.vb[0].addr;
	metaInfo.meshInfo = ss.str();
	metaInfo.distPerLoop = distPerLoop;
	for (int i = sIdx; i <= eIdx; ++i)
	{
		int idx = (isLoop && i == eIdx) ? sIdx : i;
		Parser3DAnimKey& key = mParser->mAnimKeys[idx];
		int boneCount = sizeof(key) / sizeof(key.bones[0]);
		metaInfo.boneCount = boneCount;
		mat4s mats;
		for (int j = 0; j < boneCount; ++j)
		{
			Matrix4 mat = key.bones[j].ToMat();
			mat = rotMat * mat;
			mats->push_back(mat);
		}
		metaInfo.keyMats.push_back(mats);

	}

	Parser3DAnimKey& key = mParser->mAnimKeys.front();
	int keyCount = metaInfo.keyMats.size();
	float timePerFrame = 0.0256f; //실험적 데이터(디아3기준)
	float endTime = timePerFrame * keyCount;
	metaInfo.keyCount = keyCount;
	metaInfo.endTime = endTime;

	metaInfo.SaveToFile(string(PATH_RESOURCES) + "anim/" + metaInfo.animFilename);
}

void ObjParser::ExportItemResource(string category)
{
	mParser->ReadyToExportInfo("default", false, true);
	_exceptif(mParser->mExportInfo.metaInfo.images.size() != 1, return);

	mParser->mExportInfo.ExportImage(PATH_RESOURCES + string("img/"));
	mParser->mExportInfo.ExportMeshDump(PATH_RESOURCES + string("mesh/"));

	//stringstream ss;
	//void* pTexAddr = mParser->GetTexResAddr(0);
	//ss << "*_" << pTexAddr << "_t.dump";
	//string imageName = jUtils::FindFile(PATH_PARSER_DATA + ss.str());
	//
	//ss << mParser->mContext.vb[0].addr << "_" << mFileIndex << ".dump";

	DBItemResource itemRes;
	itemRes.category = category;
	itemRes.mesh = mParser->mExportInfo.objname;
	itemRes.img = mParser->mExportInfo.metaInfo.images[0];
	itemRes.equipMesh = "null";
	itemRes.equipImg = "null";
	itemRes.uiImg = category + ".tga";
	itemRes.columnNum = category == "armor" ? 25 : (category == "mace" ? 16 : 32);
	itemRes.rowNum = category == "armor" ? 3 : (category == "gloves" || category == "pants" || category == "shoes" || category == "mace" ? 1 : 2);
	itemRes.uiImgIdx = 0;

	itemRes.Save();
}
