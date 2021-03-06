#include "jGameObjectMgr.h"
#include "jRenderer.h"
#include "jGameObject.h"

#include "ObjParser.h"
#include "ObjCamera.h"
#include "ObjGroundAxis.h"
#include "ObjTerrainMgr.h"
#include "ObjPlayer.h"
#include "ObjEnemy.h"
#include "ObjCreateHeightmap.h"
#include "ObjUI.h"
#include "ObjBomb.h"
#include "ObjItem.h"
#include "ObjHealthBars.h"
#include "oFormMain.h"
#include "oFormStats.h"
#include "oUserInputDriver.h"
#include "oCollisionMgr.h"
#include "oCuiDriver.h"
#include "oSkillIceBolt.h"

#include "jTime.h"
#include "jShader.h"
#include "jCrash.h"
#include "jInput.h"
#include "jLine3D.h"
#include "jTypeToString.h"

#include "jParserD3.h"
#include "jMesh.h"

jGameObjectMgr::jGameObjectMgr()
{
}
jGameObjectMgr::~jGameObjectMgr()
{
	Release();
}
bool jGameObjectMgr::Initialize()
{
	ID3D11Device* pDev = jRenderer::GetInst().GetDevice();
	ID3D11DeviceContext* pDevCont = jRenderer::GetInst().GetDeviceContext();

	/*
	printf("1234");
	pDev->CreateSamplerState(NULL, NULL);
	pDev->CreateTexture2D(NULL, NULL, NULL);
	pDev->CreateShaderResourceView(NULL, NULL, NULL);
	pDev->CreateBuffer(NULL, NULL, NULL);
	pDev->CreateInputLayout(NULL, 0, NULL, 0, NULL);
	pDev->CreateVertexShader(NULL, 0, NULL, NULL);
	pDev->CreatePixelShader(NULL, 0, NULL, NULL);
	
	pDevCont->OMSetBlendState(NULL, NULL, 0);
	pDevCont->OMSetDepthStencilState(NULL, 0);
	pDevCont->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDevCont->IASetIndexBuffer(NULL, DXGI_FORMAT::DXGI_FORMAT_A8_UNORM, 0);
	pDevCont->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	pDevCont->VSSetConstantBuffers(0, 0, NULL);
	pDevCont->PSSetConstantBuffers(0, 0, NULL);
	pDevCont->PSSetShaderResources(0, 0, NULL);
	pDevCont->IASetInputLayout(NULL);
	pDevCont->VSSetShader(NULL, NULL, 0);
	pDevCont->PSSetShader(NULL, NULL, 0);
	pDevCont->PSSetSamplers(0, 0, NULL);
	pDevCont->DrawIndexed(0, 0, 0);
	
	pDevCont->Map(NULL, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, NULL);
	pDevCont->Unmap(NULL, 0);
	jRenderer::GetInst().EndScene(); //Present는 jRenderer::EndScene()함수에서 확인 필요
	printf("abcd");
	*/

	//if (TestParser()) return true;

	AddGameObject(new ObjCamera());
	AddGameObject(new ObjTerrainMgr());
	AddGameObject(new ObjGroundAxis());
	AddGameObject(new ObjHealthBars());
	AddGameObject(new oUserInputDriver());
	AddGameObject(new oCollisionMgr());
	AddGameObject(new oCuiDriver());

#ifdef HeightmapCaptureMode
	AddGameObject(new ObjCreateHeightmap());
#endif
	
	AddGameObject(new ObjPlayer());
	//AddGameObject(new ObjEnemy());
	AddGameObject(new ObjUI())->SetEnable(false);
	AddGameObject(new oFormStats())->SetEnable(false);
	AddGameObject(new oFormMain())->SetEnable(false);
	
	return true;
}

bool jGameObjectMgr::TestParser()
{
	AddGameObject(new ObjCamera());
	AddGameObject(new ObjTerrainMgr());
	AddGameObject(new ObjGroundAxis());
	AddGameObject(new oUserInputDriver());
	AddGameObject(new oCollisionMgr());
	AddGameObject(new oCuiDriver());

	jParserD3::LoadResources(1);
	static vector<ObjParser*> vecObjs;
	tmpIdx = 0;
	for (int i = 99; i < 102; ++i)
	{
		ObjParser* obj0 = new ObjParser();
		obj0->mFileIndex = i;
		AddGameObject(obj0);
		vecObjs.push_back(obj0);
	}
	{
		ObjParser* obj0 = new ObjParser();
		obj0->mFileIndex = 116;
		AddGameObject(obj0);
		vecObjs.push_back(obj0);
	}

	for (int i = 0; i < 0; ++i)
	{
		jParserD3 parser;
		if (!parser.Init(i))
			continue;

		MyRes_CreateShader* pDataVS = (MyRes_CreateShader*)parser.mMapRes[parser.mContext.vs_addr].first;
		MyRes_CreateShader* pDataPS = (MyRes_CreateShader*)parser.mMapRes[parser.mContext.ps_addr].first;
		_printlog("%d vs[%d] ps[%d]\n", i, pDataVS->head.crc, pDataPS->head.crc);
		_printlog("%d vs[%p] ps[%p]\n", i, parser.mContext.vs_addr, parser.mContext.ps_addr);
		//_printlog("%d tex0[%p]\n", i, parser.mContext.tex[0].addr);
		//_printlog("%d tex1[%p]\n", i, parser.mContext.tex[1].addr);
		//_printlog("%d tex2[%p]\n", i, parser.mContext.tex[2].addr);
		//_printlog("%d tex3[%p]\n", i, parser.mContext.tex[3].addr);
		_printlog("\n");

#define TERRAIN
#ifdef TERRAIN
		if (pDataVS->head.crc == 132 || pDataVS->head.crc == 237 || pDataVS->head.crc == 194)
		{
			parser.ExportToObjectFormat("terrain", false, true);
		}
		if (pDataVS->head.crc == 99 || pDataVS->head.crc == 210 || pDataVS->head.crc == 173 || pDataVS->head.crc == 128)
		{
			parser.ExportToObjectFormat("default", false, true);
		}
		else if (pDataVS->head.crc == 154 || pDataVS->head.crc == 255 || pDataVS->head.crc == 7 || pDataVS->head.crc == 213 || pDataVS->head.crc == 22)
		{
			parser.ExportToObjectFormat("default", true, false);
		}
#endif
#ifdef SKIN
		if (pDataVS->head.crc == 83 || pDataVS->head.crc == 65)
		{
			parser.ExportToObjectFormat("skin", false, true);
		}
#endif
#ifdef ITEM
		if (pDataVS->head.crc == 22 || pDataVS->head.crc == 128)
		{
			parser.ExportToObjectFormat("default", false, true);
		}
#endif
		//252 - 주캐릭터부위별스킨
		//157,134 - 천 애니메이션
		//128 - 주캐릭아이템
		//65 - skinned 지형지물
		//215 - UI종류같음
		//213,22 - 불타는장작,커튼
	}

	jInput::GetInst().KeyDown += [this](char key) {
		if (key == '+')
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetEnable(false);
			tmpIdx = (tmpIdx + 1) % vecObjs.size();
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetEnable(true);

			jParserD3* parser = vecObjs[tmpIdx]->mParser;
			MyRes_CreateShader* pDataVS = (MyRes_CreateShader*)parser->mMapRes[parser->mContext.vs_addr].first;
			MyRes_CreateShader* pDataPS = (MyRes_CreateShader*)parser->mMapRes[parser->mContext.ps_addr].first;

			Vector3f pos;
			pos.x = parser->mCBMain.matWorld[3];
			pos.y = parser->mCBMain.matWorld[7];
			pos.z = parser->mCBMain.matWorld[11];

			_printlog("%d v[%d %d] p[%d %d] %f %f %f\n", vecObjs[tmpIdx]->mFileIndex,
				pDataVS->head.crc, pDataVS->head.totalSize,
				pDataPS->head.crc, pDataPS->head.totalSize,
				pos.x, pos.y, pos.z);
		}
		else if (key == '-')
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetEnable(false);
			tmpIdx--;
			tmpIdx = tmpIdx < 0 ? vecObjs.size() - 1 : tmpIdx;
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetEnable(true);

			jParserD3* parser = vecObjs[tmpIdx]->mParser;
			MyRes_CreateShader* pDataVS = (MyRes_CreateShader*)parser->mMapRes[parser->mContext.vs_addr].first;
			MyRes_CreateShader* pDataPS = (MyRes_CreateShader*)parser->mMapRes[parser->mContext.ps_addr].first;

			Vector3f pos;
			pos.x = parser->mCBMain.matWorld[3];
			pos.y = parser->mCBMain.matWorld[7];
			pos.z = parser->mCBMain.matWorld[11];

			_printlog("%d v[%d %d] p[%d %d] %f %f %f\n", vecObjs[tmpIdx]->mFileIndex,
				pDataVS->head.crc, pDataVS->head.totalSize,
				pDataPS->head.crc, pDataPS->head.totalSize,
				pos.x, pos.y, pos.z);
		}
	};
	return true;
}

void jGameObjectMgr::RunObjects()
{
	jTime::Update();

	for (auto it = mObjects.begin(); it != mObjects.end(); )
	{
		jGameObject* obj = it->second;
		if (obj->GetRemove())
		{
			delete obj;
			mObjects.erase(it++);
		}
		else
			++it;
	}


	for (auto iter = mNewObjects.begin(); iter != mNewObjects.end(); ++iter)
	{
		jGameObject* obj = *iter;
		if (obj->mName.length() == 0)
			obj->mName = TypeToString(obj);

		obj->LoadAll();
		mObjects.insert(make_pair(obj->mName, obj));
	}

	list<jGameObject*> newObjs = mNewObjects;
	mNewObjects.clear();
	for (jGameObject* obj : newObjs)
		obj->LoadComponents();
	for (jGameObject* obj : newObjs)
		obj->StartComponents();
	for (jGameObject* obj : newObjs)
		obj->StartAll();

	mCoroutine.RunCoroutines();


	jCrash::Clear();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		if (!obj->GetEnable())
			continue;

		obj->UpdateComponents();
		obj->UpdateAll();
	}

	vector<jShader*> shaders;
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		if (!obj->GetEnable())
			continue;

		obj->GetShaderAll(shaders);
	}

	//Sorting Rendering Order
	//layer1 : terrain
	//layer2 : terrain 바닥(바닥문양, 그림자, 스킬효과 순서) => 알파o, 깊이비교x, 깊이쓰기x  =>일단 보류...구분할 방법 없음... => layer5로 이동
	//layer3 : terrain 주변(높이가 있는 주변 지형) => 알파x, 깊이비교o, 깊이쓰기o
	//layer4 : 유닛(움직이는 유닛) => 알파x, 깊이비교o, 깊이쓰기o
	//layer5 : 높이가 있는 알파(지형, 지형 효과, 스킬효과 카메라로부터 먼순서로) => 알파o, 깊이비교o, 깊이쓰기x
	sort(shaders.begin(), shaders.end(), [](jShader* rhs, jShader* lhs) {
		return rhs->GetRenderOrder() < lhs->GetRenderOrder();
	});

	ObjCamera * cam = (ObjCamera *)FindGameObject("ObjCamera");
	jRenderer::GetInst().BeginScene();
	for (jShader* shader : shaders)
		shader->OnRender(cam);
	jRenderer::GetInst().EndScene();
	shaders.clear();
}
void jGameObjectMgr::Release()
{
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		delete obj;
		obj = nullptr;
	}
	mObjects.clear();

}

void jGameObjectMgr::StopCoRoutine(string name)
{
	mCoroutine.StopCoroutine(name);
}
void jGameObjectMgr::StartCoRoutine(string name, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Normal;
	info.name = name;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
	mCoroutine.StartCoroutine(info);
}
void jGameObjectMgr::StartCoRoutine(string name, float time_ms, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Timer;
	info.name = name;
	info.time_ms = time_ms;
	info.time_back_ms = time_ms;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
	mCoroutine.StartCoroutine(info);
}

//name : 코루틴 이름, task : 쓰레드에서 수행되는 함수, coroutine : 쓰레드 완료시 수행되는 코루틴
void jGameObjectMgr::StartCoRoutine(string name, std::function<void(void)> task, std::function<CorCmd(CorMember&, bool)> coroutine)
{
	CoroutineInfo info;
	info.enabled = true;
	info.mode = CorMode::Task;
	info.name = name;
	info.task = task;
	info.taskStarted = false;
	info.taskDone = false;
	info.firstCalled = true;
	info.coroutine = coroutine;
	info.pThread = nullptr;
	mCoroutine.StartCoroutine(info);
}

jGameObject* jGameObjectMgr::RayCast(Vector3 pos, Vector3 dir, jGameObject* skipObject)
{
	jLine3D line(pos, pos + dir);
	
	jGameObject* ret = nullptr;
	double minDist = 1000000.0;
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		jCrash* crash = obj->FindComponent<jCrash>();
		if (crash == nullptr || !crash->GetEnable())
			continue;

		if (!crash->IsCrash(line))
			continue;

		if (skipObject == obj)
			continue;

		double distFar = obj->GetTransform().getPos().distance(pos);
		if (distFar < minDist)
		{
			minDist = distFar;
			ret = obj;
		}
	}
	return ret;
}

jGameObject* jGameObjectMgr::AddGameObject(jGameObject* _obj)
{
	mNewObjects.push_back(_obj);
	return _obj;
}
bool jGameObjectMgr::Added(jGameObject * obj)
{
	string name = obj->GetName();
	jGameObjects objs = FindGameObjects(name);
	for (jGameObject * candi : *objs)
		if (candi == obj)
			return true;
	return false;
}
jGameObject* jGameObjectMgr::FindGameObject(string objectName)
{
	if (mObjects.find(objectName) == mObjects.end())
		return nullptr;

	return mObjects.find(objectName)->second;
}

jGameObjects jGameObjectMgr::FindGameObjects(string objectName)
{
	jGameObjects ret;
	if (mObjects.find(objectName) == mObjects.end())
		return ret;

	auto boundary = mObjects.equal_range(objectName);
	for (auto iter = boundary.first; iter != boundary.second; ++iter)
		ret->push_back(iter->second);

	return ret;
}

