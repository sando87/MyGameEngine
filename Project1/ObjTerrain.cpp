#include "ObjTerrain.h"
#include "jLog.h"
#include "jModel.h"
#include "jTexture.h"
#include "jShaderTerrain.h"
#include "jGameObjectMgr.h"
#include "jUtils.h"
#include "jAStar.h"
#include "jHeightMap.h"

ObjTerrain::ObjTerrain()
{
	mModel = nullptr;
	mShader = nullptr;
	for(int i = 0; i < 6; ++i)
		mTexture[i] = nullptr;

	mFileIndex = 0;
}


ObjTerrain::~ObjTerrain()
{
	if (mModel)
		mModel->Release();
	if (mShader)
		mShader->Release();
	if (mHeightMap)
		delete mHeightMap;
	for (int i = 0; i < 6; ++i)
		if (mTexture[i])
			mTexture[i]->Release();
}


void ObjTerrain::OnStart()
{

	//if (!mRenderIfno.Init(mFileIndex))
	//{
	//	_printlog("fail to load [%d]\n", mFileIndex);
	//	DeleteFromMgr();
	//	return;
	//}
	//
	//if (!mRenderIfno.IsTerrain() || mRenderIfno.mTextures.size() < 6)
	//{
	//	printf("[%d] This isn't Terrain Floor\n", mFileIndex);
	//	DeleteFromMgr();
	//	return;
	//}
	//
	//_printlog("Terrain[%d]\n", mFileIndex);

	//mRenderIfno.ExportToObjectFormat();

	//mModel = new jModel();
	//mModel->LoadDiablo_ForTerrain(&mRenderIfno);
	//mModel->LoadHeightMap("heightMapSample.tga", 1.0f, 50.0f);

	//jHeightMap map(256, 256, 1);
	//map.CreateHeightMap(mModel);
	//jAStar Astar;
	//Astar.SetHeightMap(&map);
	//Astar.Route(Vector2f(0, 0), Vector2f(0, 128));

	//for (int i = 0; i < 6; ++i)
	//{
	//	ID3D11ShaderResourceView* pIF = mRenderIfno.GetResShaderResourceView(i);
	//	mTexture[i] = new jTexture();
	//	mTexture[i]->SetShaderResourceView(pIF);
	//}
	//
	//mShader = new jShaderTerrain();
	//mShader->Initialize("./terrain.vs", "./terrain.ps");
	//
	//for (int i = 0; i < 6; ++i)
	//{
	//	Matrix4f& matTex = mRenderIfno.mCBMain.matTex[i +1];
	//	mTexels[i * 2 + 0] = Vector4f(matTex[0], matTex[1], matTex[3], 0);
	//	mTexels[i * 2 + 1] = Vector4f(matTex[4], matTex[5], matTex[7], 0);
	//}
}

void ObjTerrain::OnUpdate()
{

}

void ObjTerrain::OnDraw()
{
	//Matrix4 mat = Matrix4().identity();
	//
	//Matrix4 CBmat = mRenderIfno.mCBMain.matWorld;
	//CBmat.transpose();
	//static map<Vector3f, int> mmTest;
	//Vector3f key;
	//key.x = CBmat[12];
	//key.y = CBmat[13];
	//key.z = CBmat[14];
	//if (mmTest.find(key) == mmTest.end())
	//{
	//	printf("[%d] x: %f, y: %f, z: %f\n", mFileIndex, CBmat[12], CBmat[13], CBmat[14]);
	//	mmTest[key] = 1;
	//}
	//
	//mat[12] = CBmat[12];
	//mat[13] = CBmat[13];
	//mat[14] = CBmat[14];


	for (TerrainSubObject* obj : mSubObjects)
	{
		Matrix4 mat = Matrix4().identity();
		mat[12] = obj->position.x;
		mat[13] = obj->position.y;
		mat[14] = obj->position.z;
		mShader->SetParams(obj->model, mat, &obj->textures.front(), &obj->texels.front());
		mShader->Render();
	}
}

void ObjTerrain::Load(vector<string>* names, float size, float step)
{
	if (mHeightMap == nullptr)
		mHeightMap = new jHeightMap(size, size, step);

	for (string name : *names)
	{
		TerrainSubObject* subObj = new TerrainSubObject();
		subObj->Load(name, size);

		mHeightMap->UpdateHeightMap(subObj->model, subObj->position.z);
		
		float minHeight = mHeightMap->MinHeight();
		float maxHeight = mHeightMap->MaxHeight();
		Vector3 minPt(subObj->position.x, subObj->position.y, minHeight);
		Vector3 maxPt(subObj->position.x + size, subObj->position.y + size, maxHeight);
		mRect.expand(minPt);
		mRect.expand(maxPt);
		
		mSubObjects.push_back(subObj);
	}

	mShader = new jShaderTerrain();
	mShader->Initialize("./terrain.vs", "./terrain.ps");
}
float ObjTerrain::GetHeight(float x, float y)
{
	jRect rect = mRect.TopBottom();
	return mHeightMap->GetHeightOfPos(x - rect.Left(), y - rect.Bottom());
}

bool TerrainSubObject::Load(string _name, int size)
{
	name = _name;
	string path = "D:\\export\\D3\\" + name +"\\";

	model = new jModel();
	model->Load(path + name + ".obj");

	strings lines = jUtils::LoadLines(path + name + ".txt");
	strings pieces = jUtils::Split2(lines[0], " ");
	position = Vector3(jUtils::ToDouble(pieces[0]), jUtils::ToDouble(pieces[1]), jUtils::ToDouble(pieces[2]));
	position.x = ((int)position.x / size) * size;
	position.y = ((int)position.y / size) * size;

	for (int i = 1; i < lines->size(); ++i)
	{
		strings columns = jUtils::Split2(lines[i], " ");
		if (columns->size() >= 3)
		{
			Vector4f pt;
			pt.x = jUtils::ToDouble(columns[0]);
			pt.y = jUtils::ToDouble(columns[1]);
			pt.z = jUtils::ToDouble(columns[2]);
			pt.w = 0;
			texels.push_back(pt);
		}
		else
		{
			string tgaImageName = columns[0];
			jTexture* texture = new jTexture();
			texture->Initialize(path + tgaImageName);
			textures.push_back(texture);
		}
	}
	return true;
}