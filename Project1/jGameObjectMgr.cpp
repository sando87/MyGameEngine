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

#include "jTime.h"
#include "jShader.h"
#include "jCrash.h"
#include "jInput.h"
#include "jLine3D.h"

#include "jParserD3.h"
#include "jMesh.h"

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

	(new ObjGroundAxis())->AddToMgr();
	(new ObjCreateHeightmap())->AddToMgr();

	//(new ObjPlayer())->AddToMgr();
	//(new ObjEnemy())->AddToMgr();

	static vector<ObjParser*> vecObjs;
	tmpIdx = 0;
	for(int i = 157; i < 0; ++i)
	{
		ObjParser* obj0 = new ObjParser();
		obj0->mFileIndex = i;
		//obj0->mOff = (i - 192) * 20.0f;
		obj0->AddToMgr();
		vecObjs.push_back(obj0);
	}

	//jParserD3::LoadResources(1);
	for (int i = 250; i < 0; ++i)
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
		//252 - 주캐릭터부위별스킨
		//157,134 - 천 애니메이션
		//128 - 주캐릭아이템(방패)
		//65 - skinned 지형지물
		//215 - UI종류같음
		//213,22 - 불타는장작,커튼
	}



	jInput::GetInst().mKeyboard += [this](const unsigned char* key) {
		//char keys[256] = { 0, };
		//memcpy(keys, key, 256);
		
		if (key[78] != 0)
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(false);
			tmpIdx = (tmpIdx + 1) % vecObjs.size();
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(true);

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
			Sleep(200);
		}
		else if (key[74] != 0)
		{
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(false);
			tmpIdx--;
			tmpIdx = tmpIdx < 0 ? vecObjs.size() - 1 : tmpIdx;
			vecObjs[tmpIdx]->FindComponent<jShader>()->SetVisiable(true);

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
			Sleep(200);
		}
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


	mCoroutine.RunCoroutines();

	for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
	{
		if (!(*it)->mIsRemoved)
			(*it)->OnUpdate();

		jCrash* crash = (*it)->FindComponent<jCrash>();
		if (crash != nullptr)
		{
			jRect3D rect = crash->GetRect();
			vector<list<jCrash*>*> grids;
			mCrashGrid.GetGrids(rect.TopBottom().ClipMinus(), grids);
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
			crash->CallbackCrash();
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

		jRect3D rect = crash->GetRect();
		vector<list<jCrash*>*> grids;
		mCrashGrid.GetGrids(rect.TopBottom().ClipMinus(), grids);
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

void jGameObjectMgr::StartCoroutine(CoroutineInfo coroutineInfo)
{ 
	mCoroutine.StartCoroutine(coroutineInfo);
}
void jGameObjectMgr::StopCoroutine(string name)
{
	mCoroutine.StopCoroutine(name);
}

jGameObject* jGameObjectMgr::RayCast(Vector3 pos, Vector3 dir)
{
	jLine3D line(pos, dir);
	jLine3D line2D(Vector3(pos.x, pos.y, 0), Vector3(dir.x, dir.y, 0));
	
	jGameObject* ret = nullptr;
	double minDist = 1000.0;
	for (jGameObject* obj : mObjects)
	{
		jCrash* crash = obj->FindComponent<jCrash>();
		if (crash == nullptr)
			continue;

		Vector3 objPos = obj->GetTransport().getPos();
		Vector3 onPT = line2D.ClosePoint(Vector3(objPos.x, objPos.y, 0));
		double distOfShape = onPT.distance(Vector3(objPos.x, objPos.y, 0));
		if (distOfShape > crash->GetShape().round)
			continue;

		jRect3D rt = crash->GetRect();
		Vector2 posYZ = line.GetYZ(onPT.x);
		if (rt.Min().z > posYZ.y || posYZ.y > rt.Max().z)
			continue;

		double distFar = mCamera->GetTransport().getPos().distance(onPT);
		if (distFar < minDist)
		{
			minDist = distFar;
			ret = obj;
		}
	}
	return ret;
}
