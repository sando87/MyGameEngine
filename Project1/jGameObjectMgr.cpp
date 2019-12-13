#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"
#include "ObjTutorial1.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"
#include "ObjDiablo.h"
#include "jUtils.h"
#include "jGlobalStruct.h"
#include "ObjTerrain.h"
#include "jObjStarCraft.h"
#include "ObjTerrainMgr.h"
#include "ObjPlayer.h"
#include "jLog.h"
#include "jTime.h"
#include "jComponent.h"
#include "jMesh.h"
#include "jShaderSkin.h"

jGameObjectMgr::jGameObjectMgr()
{
	mCamera = nullptr;
	mTerrainMgr = nullptr;
}


jGameObjectMgr::~jGameObjectMgr()
{
	Release();
}

bool jGameObjectMgr::Initialize()
{
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevCont = jRenderer::GetInst().GetDeviceContext();

	//pDev->CreateSamplerState(NULL, NULL);
	//pDev->CreateTexture2D(NULL, NULL, NULL);
	//pDev->CreateShaderResourceView(NULL, NULL, NULL);
	//pDev->CreateBuffer(NULL, NULL, NULL);
	//pDev->CreateInputLayout(NULL, 0, NULL, 0, NULL);
	//pDev->CreateVertexShader(NULL, 0, NULL, NULL);
	//pDev->CreatePixelShader(NULL, 0, NULL, NULL);
	//
	//pDevCont->OMSetBlendState(NULL, NULL, 0);
	//pDevCont->OMSetDepthStencilState(NULL, 0);
	//pDevCont->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	//pDevCont->IASetIndexBuffer(NULL, DXGI_FORMAT::DXGI_FORMAT_A8_UNORM, 0);
	//pDevCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	//pDevCont->VSSetConstantBuffers(0, 0, NULL);
	//pDevCont->PSSetConstantBuffers(0, 0, NULL);
	//pDevCont->PSSetShaderResources(0, 0, NULL);
	//pDevCont->IASetInputLayout(NULL);
	//pDevCont->VSSetShader(NULL, NULL, 0);
	//pDevCont->PSSetShader(NULL, NULL, 0);
	//pDevCont->PSSetSamplers(0, 0, NULL);
	//pDevCont->DrawIndexed(0, 0, 0);
	//
	//pDevCont->Map(NULL, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, NULL);
	//pDevCont->Unmap(NULL, 0);
	//Present는 jRenderer::EndScene()함수에서 확인 필요
	//00007FFEA8D34300
	//00007FFEA8D30000

	mCamera = new ObjCamera();
	mCamera->AddToMgr();

	ObjGroundAxis* obj = new ObjGroundAxis();
	obj->AddToMgr();

	mTerrainMgr = new ObjTerrainMgr();
	mTerrainMgr->AddToMgr();
	
	(new ObjPlayer())->AddToMgr();

	//for(int i = 134; i < 136; ++i)
	//{
	//	ObjDiablo* obj0 = new ObjDiablo();
	//	obj0->mFileIndex = i;
	//	obj0->AddToMgr();
	//}


	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 413; obj0->AddToMgr(); } //케리어
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 424; obj0->AddToMgr(); } //커세어
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 425; obj0->AddToMgr(); } //파수기
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 426; obj0->AddToMgr(); } //사도
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 427; obj0->AddToMgr(); } //하템
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 431; obj0->AddToMgr(); } //셔틀
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 436; obj0->AddToMgr(); } //
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 438; obj0->AddToMgr(); } //드라군
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 439; obj0->AddToMgr(); } //
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 448; obj0->AddToMgr(); } //프로브
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 449; obj0->AddToMgr(); } //질럿
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 450; obj0->AddToMgr(); } //불멸자
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 456; obj0->AddToMgr(); } //거신
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 457; obj0->AddToMgr(); } //폭풍함
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 463; obj0->AddToMgr(); } //스카웃
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 568; obj0->AddToMgr(); } //
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 569; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 570; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 571; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 572; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 573; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 592; obj0->AddToMgr(); } //옵져버
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 593; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 594; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 595; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 596; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 597; obj0->AddToMgr(); }
	//{ jObjStarCraft* obj0 = new jObjStarCraft(); obj0->mFileIndex = 598; obj0->AddToMgr(); }
	return true;
}

void jGameObjectMgr::RunObjects()
{
	jTime::Update();

	for (auto it = mObjects.begin(); it != mObjects.end(); )
	{
		jGameObject* obj = *it;
		if (obj->mIsRemoved)
		{
			delete obj;
			mObjects.erase(it++);
		}
		else
			++it;
	}

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		if ((*it)->mIsStarted)
			continue;

		(*it)->OnStart();
		(*it)->mIsStarted = true;
	}

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		list<jComponent*>& components = (*it)->mComponents;
		for (jComponent* comp : components)
		{
			jShader* shader = dynamic_cast<jShader*>(comp);
			if (shader)
				shader->LoadRenderResources();
		}
	}

	for (auto iter = mCoroutines.begin(); iter != mCoroutines.end();)
	{
		function<bool(void)> func = iter->second;
		if (func == nullptr)
			mCoroutines.erase(iter++);
		else if (func())
			++iter;
		else
			mCoroutines.erase(iter++);
	}

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
		if(!(*it)->mIsRemoved)
			(*it)->OnUpdate();

	jRenderer::GetInst().BeginScene();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
		if (!(*it)->mIsRemoved)
			(*it)->OnDraw();
	jRenderer::GetInst().EndScene();
}

bool LoadResources(void *_this, char *_filename)
{
	jGameObjectMgr* pObj = (jGameObjectMgr*)_this;
	vector<string> rets;
	jUtils::Split(string(_filename), "_", rets);
	if (stoi(rets[0]) != pObj->mFileIndex)
		return true;

	if (rets[1] == "texture")
	{
		int slot = stoi(rets[4]);
		string filename = PATH_RESOURCE + string(_filename);

		FILE *pFile = NULL;
		fopen_s(&pFile, filename.c_str(), "rb");
		if (pFile == NULL)
			return true;

		MyResBase head;
		fread_s(&head, sizeof(head), sizeof(head), 1, pFile);

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}

		string tgaName = PATH_RESOURCE + string(_filename) + ".tga";
		fopen_s(&pFile, tgaName.c_str(), "rb");
		if (pFile == NULL)
			return true;

		fseek(pFile, 0, SEEK_END);
		int filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		MyRes_CreateTexture *pData = (MyRes_CreateTexture *)malloc(sizeof(MyRes_CreateTexture) + filesize);
		memset(pData, 0x00, sizeof(MyRes_CreateTexture) + filesize);
		fread_s(pData->data, filesize, filesize, 1, pFile);
		pData->head = head;
		pData->head.totalSize = sizeof(MyRes_CreateTexture) + filesize;
		pData->head.crc = slot;

		pObj->AddResource(&pData->head);

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}
	else
	{
		string path = PATH_RESOURCE;
		path += _filename;

		FILE *pFile = NULL;
		fopen_s(&pFile, path.c_str(), "rb");
		if (pFile == NULL)
			return true;

		fseek(pFile, 0, SEEK_END);
		int filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		MyResBase *pData = (MyResBase*)malloc(filesize);
		fread_s(pData, filesize, filesize, 1, pFile);
		char ch = ((char*)pData)[1];
		
		if(ch == 0)
			pObj->AddResource(pData);
		else
		{
			free(pData);
			pData = NULL;
		}

		if (pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}

	return true;
}
void jGameObjectMgr::AddResource(MyResBase* _res)
{
	mResources[_res->addr] = _res;
}
MyResBase* jGameObjectMgr::GetResource(void* _res)
{
	if (mResources.find(_res) == mResources.end())
		return NULL;
	return mResources[_res];
}
bool jGameObjectMgr::AddGPURes(MyResBase* pData, void* pInterface)
{
	mGPURes[pData->addr] = make_pair(pData, pInterface);
	return true;
}
bool jGameObjectMgr::InitResources(int _idx)
{
	string path = PATH_RESOURCE;
	path += "*.dump";
	mFileIndex = _idx;
	jUtils::ForEachFiles(this, path.c_str(), LoadResources);
	return true;
}

void jGameObjectMgr::Release()
{
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = *it;
		delete obj;
		obj = nullptr;
	}
	mObjects.clear();

	for (auto it = mResources.begin(); it != mResources.end(); ++it)
	{
		MyResBase* obj = it->second;
		free(obj);
		obj = nullptr;
	}
	mResources.clear();
}
void jGameObjectMgr::StartCoroutine(function<bool(void)> coroutine)
{
	static u32 index = 0;
	index++;
	string key = "__" + jUtils::ToString(index) + "__";
	mCoroutines[key] = coroutine;
}
void jGameObjectMgr::StartCoroutine(function<bool(void)> coroutine, string name)
{ 
	mCoroutines[name] = coroutine;
}
void jGameObjectMgr::StopCoroutine(string name)
{
	mCoroutines[name] = nullptr;
}
