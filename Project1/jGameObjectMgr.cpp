#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"

#include "ObjParser.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"
#include "ObjTerrainMgr.h"
#include "ObjPlayer.h"

#include "jTime.h"
#include "jShader.h"
#include "jCrash.h"
#include "jInput.h"

#include "jParserD3.h"

jGameObjectMgr::jGameObjectMgr() : mCrashGrid(CRASH_SIZE, CRASH_STEP)
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

	mTerrainMgr = new ObjTerrainMgr();
	mTerrainMgr->AddToMgr();

	mCamera = new ObjCamera();
	mCamera->AddToMgr();
	
	ObjGroundAxis* obj = new ObjGroundAxis();
	obj->AddToMgr();
	
	//(new ObjPlayer())->AddToMgr();

	static vector<ObjParser*> vecObjs;
	tmpIdx = 0;
	//for(int i = 203; i < 210; ++i)
	//{
	//	ObjParser* obj0 = new ObjParser();
	//	obj0->mFileIndex = i;
	//	//obj0->mOff = (i - 203) * 20.0f;
	//	obj0->AddToMgr();
	//	vecObjs.push_back(obj0);
	//}

	//{ jParserD3 parser; parser.Init(203);	parser.ExportToObjectFormat("terrain", false, true);}
	//{ jParserD3 parser; parser.Init(204);	parser.ExportToObjectFormat("terrain", false, true);}
	//{ jParserD3 parser; parser.Init(206);	parser.ExportToObjectFormat("default", false, true);}
	//{ jParserD3 parser; parser.Init(207);	parser.ExportToObjectFormat("default", false, true);}
	//{ jParserD3 parser; parser.Init(208);	parser.ExportToObjectFormat("default", false, true);}


	jInput::GetInst().mKeyboard += [this](const unsigned char* key) {
		//char keys[256] = { 0, };
		//memcpy(keys, key, 256);
		
		if (key[78] != 0)
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(false);
			tmpIdx = (tmpIdx + 1) % vecObjs.size();
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(true);
			_echoN(vecObjs[tmpIdx]->mFileIndex);
		}
		else if (key[74] != 0)
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(false);
			tmpIdx--;
			tmpIdx = tmpIdx < 0 ? vecObjs.size() - 1 : tmpIdx;
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(true);
			_echoN(vecObjs[tmpIdx]->mFileIndex);
		}
		Sleep(500);
		// enter key[41]
		// a key[30]
		// 1 key[2]
		// num9 key[73]
		// unm6 key[77]
		// + key[78]
		// - key[74]
	};

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
	{
		if (!(*it)->mIsRemoved)
			(*it)->OnUpdate();

		jCrash* crash = (*it)->FindComponent<jCrash>();
		if (crash != nullptr)
		{
			jRect rect = crash->GetRect();
			vector<list<jCrash*>*> grids;
			mCrashGrid.GetGrids(rect, grids);
			for (list<jCrash*>* grid : grids)
				grid->push_back(crash);
		}
	}
		

	AddCrashs();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = *it;
		jCrash* crash = obj->FindComponent<jCrash>();
		if (crash != nullptr)
			crash->OnCrash();
	}

	jRenderer::GetInst().BeginScene();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = *it;
		if (!obj->mIsRemoved)
		{
			jShader* shader = obj->FindComponent<jShader>();
			if (shader != nullptr)
			{
				if (!shader->mLoaded)
				{
					shader->OnLoad();
					shader->mLoaded = true;
				}

				if(shader->GetVisiable())
					shader->OnRender();
			}
		}
	}
	jRenderer::GetInst().EndScene();
}
void jGameObjectMgr::AddCrashs()
{
	//우선 게임 오브젝트 별로 루프를 돌며
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = *it;
		if (obj->mIsRemoved)
			continue;

		jCrash* crash = obj->FindComponent<jCrash>();
		if (crash == nullptr)
			continue;

		jRect rect = crash->GetRect();
		vector<list<jCrash*>*> grids;
		mCrashGrid.GetGrids(rect, grids);
		//현재 오브젝트의 걸친 충돌 grid들을 찾고
		for (list<jCrash*> *grid : grids)
		{
			//각각의 grid안에 있는 오브젝트 충돌체를 검사한다.
			list<jCrash*>::iterator me;
			for (auto iter = grid->begin(); iter != grid->end(); ++iter)
			{
				jCrash* target = *iter;
				if (crash == target)
				{
					me = iter;
					continue;
				}

				if (crash->Crashed(target))
				{
					crash->AddCrashed(target);
					target->AddCrashed(crash);
				}
			}
			grid->erase(me);
		}
	}
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
