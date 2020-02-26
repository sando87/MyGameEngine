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
#include "ObjEffect.h"
#include "ObjBomb.h"

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
	//Present�� jRenderer::EndScene()�Լ����� Ȯ�� �ʿ�
	//00007FFEA8D34300
	//00007FFEA8D30000

	AddGameObject(new ObjGroundAxis());
	//AddGameObject(new ObjCreateHeightmap());

	//AddGameObject(new ObjPlayer());
	//AddGameObject(new ObjEnemy());
	//AddGameObject(new ObjUI());
	//AddGameObject(new ObjEffect());
	AddGameObject(new ObjBomb());

	//jParserD3::LoadResources(1);
	static vector<ObjParser*> vecObjs;
	tmpIdx = 0;
	for(int i = 0; i < 0; ++i)
	{
		ObjParser* obj0 = new ObjParser();
		obj0->mFileIndex = i;
		obj0->mOff = (i - 0) * 20.0f;
		AddGameObject(obj0);
		vecObjs.push_back(obj0);
	}
	int objs[] = { 103, 105, 108 };
	for (int i = 0; i < 0; ++i)
	{
		ObjParser* obj0 = new ObjParser();
		obj0->mFileIndex = objs[i];
		obj0->mOff = (objs[i] - 0) * 20.0f;
		AddGameObject(obj0);
		vecObjs.push_back(obj0);
	}

	for (int i = 0; i < 0; ++i)
	{
		jParserD3 parser; 
		if (!parser.Init(i))
			continue;

		MyRes_CreateShader* pDataVS = (MyRes_CreateShader*)parser.mMapRes[parser.mContext.vs_addr].first;
		_printlog("%d v[%d]\n", i, pDataVS->head.crc);
		if (pDataVS->head.crc == 132 || pDataVS->head.crc == 237 || pDataVS->head.crc == 194)
		{
			parser.ExportToObjectFormat("terrain", false, true);
		}
		if (pDataVS->head.crc == 99 || pDataVS->head.crc == 210 || pDataVS->head.crc == 173)
		{
			parser.ExportToObjectFormat("default", false, true);
		}
		else if (pDataVS->head.crc == 154 || pDataVS->head.crc == 255 || pDataVS->head.crc == 7 || pDataVS->head.crc == 213 || pDataVS->head.crc == 22)
		{
			parser.ExportToObjectFormat("default", true, false);
		}
		//else if (pDataVS->head.crc == 83 || pDataVS->head.crc == 65)
		//{
		//	parser.ExportToObjectFormat("skin", false, true);
		//}
		//252 - ��ĳ���ͺ�������Ų
		//157,134 - õ �ִϸ��̼�
		//128 - ��ĳ��������(����)
		//65 - skinned ��������
		//215 - UI��������
		//213,22 - ��Ÿ������,Ŀư
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

		obj->OnLoad();
		mObjects.insert(make_pair(obj->mName, obj));
	}

	list<jGameObject*> newObjs = mNewObjects;
	mNewObjects.clear();
	for (auto iter = newObjs.begin(); iter != newObjs.end(); ++iter)
	{
		jGameObject* obj = *iter;
		obj->LoadComponents();
		obj->OnStart();
	}

	mCoroutine.RunCoroutines();


	jCrash::Clear();
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		obj->UpdateComponents();
		obj->OnUpdate();
	}

	vector<jShader*> shaders;
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		jShader* shader = obj->FindComponent<jShader>();
		if (shader == nullptr)
			continue;

		if (shader->GetEnable())
			shaders.push_back(shader);
	}

	//Sorting Rendering Order
	//layer1 : terrain
	//layer2 : terrain �ٴ�(�ٴڹ���, �׸���, ��ųȿ�� ����) => ����o, ���̺�x, ���̾���x  =>�ϴ� ����...������ ��� ����... => layer5�� �̵�
	//layer3 : terrain �ֺ�(���̰� �ִ� �ֺ� ����) => ����x, ���̺�o, ���̾���o
	//layer4 : ����(�����̴� ����) => ����x, ���̺�o, ���̾���o
	//layer5 : ���̰� �ִ� ����(����, ���� ȿ��, ��ųȿ�� ī�޶�κ��� �ռ�����) => ����o, ���̺�o, ���̾���x
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

//name : �ڷ�ƾ �̸�, task : �����忡�� ����Ǵ� �Լ�, coroutine : ������ �Ϸ�� ����Ǵ� �ڷ�ƾ
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

jGameObject* jGameObjectMgr::RayCast(Vector3 pos, Vector3 dir)
{
	jLine3D line(pos, dir);
	
	jGameObject* ret = nullptr;
	double minDist = 1000000.0;
	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		jGameObject* obj = it->second;
		jCrash* crash = obj->FindComponent<jCrash>();
		if (crash == nullptr)
			continue;

		if (!crash->IsCrash(line))
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

void jGameObjectMgr::AddGameObject(jGameObject* _obj)
{
	mNewObjects.push_back(_obj);
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

