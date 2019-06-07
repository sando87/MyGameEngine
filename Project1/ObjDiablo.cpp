#include "ObjDiablo.h"
#include "jModel.h"
#include "ObjCamera.h"
#include "jShaderDiablo.h"
#include "jShaderColor.h"
#include "jShaderSkinned.h"
#include "jTexture.h"
#include "jShaderTexture.h"
#include "jGameObjectMgr.h"
#include "jUtils.h"
#include <sstream>
#include <iostream>

ObjDiablo::ObjDiablo()
{
	mShader = nullptr;
	mModel = nullptr;
}


ObjDiablo::~ObjDiablo()
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

void ObjDiablo::OnStart()
{
	mDrawInfos = DoDrawingInfo(1);

	mModel = new jModel();
	mModel->LoadDiablo_FromRes(mDrawInfos.vertexBuffer[0], mDrawInfos.layout);
	mModel->LoadDiablo_FromRes(mDrawInfos.indexBuffer, mDrawInfos.layout);

	mTexture = new jTexture();
	mTexture->Load_FromRes(mDrawInfos.texture[0]);

	mShader = new jShaderTexture();
	mShader->Initialize("./texture.vs", "./texture.ps");

	//mShader = new jShaderSkinned();
	//mShader->Initialize("./test.vs", "./test.ps");
	//
	//mTexture = new jTexture();
	//mTexture->Initialize("./Texture2D_0_1_0000015D1A1E9468.tga");
}

void ObjDiablo::OnUpdate()
{
}

void ObjDiablo::OnDraw()
{
	Matrix4 mat = Matrix4().identity();
	//mat[12] = -50.0f;
	//mat[13] = -50.0f;
	//mat[14] = -70.0f;
	//mShader->SetParams(mModel, mat, &GetCamera(), false);
	//mShader->Render();

	//vector<Matrix4> mats;
	//mats.push_back(mat);
	//mShader->SetParams(mModel, mat, &GetCamera(), mTexture, Vector4f(0.5f, 0.5f, 0.5f, 1.0f), Vector4f(1, 1, -1, 0), mats);
	//mShader->Render();

	mShader->SetParams(mModel, mat, mTexture);
	mShader->mIndexCount = mDrawInfos.IndexCount;
	mShader->mStartIndex = mDrawInfos.StartIndex;
	mShader->mVertexOff = mDrawInfos.VertexOffset;
	mShader->Render();
}

DrawingInfo ObjDiablo::DoDrawingInfo(int _idx)
{
	DrawingInfo ret = { 0, };
	string path = "D:\\last\\DrawingInfo.txt";

	FILE *pFile = NULL;
	fopen_s(&pFile, path.c_str(), "rb");
	if (pFile == NULL)
		return ret;

	fseek(pFile, 0, SEEK_END);
	int filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *pData = (char*)malloc(filesize);
	fread_s(pData, filesize, filesize, 1, pFile);

	string data = pData;
	vector<string> lines;
	jUtils::Split(data, "\r\n", lines);
	int cnt = lines.size();
	bool find = false;
	for (int i = 0; i < cnt; ++i)
	{
		string line = lines[i];
		if (find)
		{
			if (line.c_str()[0] == '=')
				break;

			vector<string> infos;
			jUtils::Split(line, "_", infos);
			if (infos[0] == "VertexShader")
			{
				string addr = infos[3];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.vertexShader = pData;
			}
			else if (infos[0] == "PixelShader")
			{
				string addr = infos[3];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.pixelShader = pData;
			}
			else if (infos[0] == "texture")
			{
				string slot = infos[3];
				string addr = infos[5];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.texture[stoi(slot)] = pData;
			}
			else if (infos[0] == "IB")
			{
				string addr = infos[3];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.indexBuffer = pData;
			}
			else if (infos[0] == "VB")
			{
				string slot = infos[3];
				string addr = infos[5];
				string stride = infos[6];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.vertexBuffer[stoi(slot)] = pData;
			}
			else if (infos[0] == "layout")
			{
				string addr = infos[3];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.layout = pData;
			}
			else if (infos[0] == "Primitive")
			{
				string num = infos[1];
				ret.primitive = stoi(num);
			}
			else if (infos[0] == "Draw")
			{
				ret.IndexCount = stoi(infos[1]);
				ret.StartIndex = stoi(infos[2]);
				ret.VertexOffset = stoi(infos[3]);
			}
			else if (infos[0] == "Sampler")
			{
				string slot = infos[3];
				string addr = infos[5];
				unsigned long long nAddr = 0;
				stringstream ss;
				ss << std::hex << addr;
				ss >> nAddr;
				MyResBase* pData = jGameObjectMgr::GetInst().GetResource((void*)nAddr);
				ret.sampler[stoi(slot)] = pData;
			}
		}
		else if (!find && line.c_str()[0] == '=')
		{
			vector<string> infos;
			jUtils::Split(line, " ", infos);
			string idx = infos[1];
			if (stoi(idx) == _idx)
			{
				find = true;
				continue;
			}
		}
	}

	if (pFile)
		fclose(pFile);
	if (pData)
		free(pData);

	return ret;
}
