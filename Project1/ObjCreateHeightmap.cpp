#include "ObjCreateHeightmap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderColor.h"
#include "jRenderer.h"
#include "jBitmap.h"
#include "ObjCamera.h"
#include "jZMapLoader.h"
#include "jParserMeta.h"
#include "jShaderHeader.h"
#include "jInputEvent.h"

class jEventHeightMap : public jInputEvent
{
private:
	int mIndex = 0;
	virtual void OnKeyDown(char ch);
};

#define CONF_Width (640)
#define CONF_Height (480)
#define CONF_Step (1)
#define CONF_Size (240)


ObjCreateHeightmap::ObjCreateHeightmap()
{
}


ObjCreateHeightmap::~ObjCreateHeightmap()
{
}

void ObjCreateHeightmap::OnStart()
{
	AddComponent(new jEventHeightMap());
	LoadBlocksInfo();
}

void ObjCreateHeightmap::LoadBlocksInfo()
{
	string path = PATH_RESOURCES + string("meta/");
	jUtils::ForEachFiles2(nullptr, (path + "*").c_str(), [&](void *obj, string foldername) {
		strings coordinates = jUtils::Split2(foldername, "_");
		if (coordinates->size() != 2)
			return true;

		int x = atoi(coordinates[0].c_str());
		int y = atoi(coordinates[1].c_str());
		
		double minHeight = 1000;
		double maxHeight = -1000;
		jUtils::ForEachFiles2(nullptr, (path + foldername + "/*.txt").c_str(), [&](void *obj, string filename) {
			Vector2 minmax;
			bool ret = FindMinMaxHeight(path + foldername + "/" + filename, minmax);
			if (ret)
			{
				minHeight = min(minHeight, minmax.x);
				maxHeight = max(maxHeight, minmax.y);
			}
			return true;
		});

		mBlocks.push_back(Vector4n(x, y, minHeight-1, maxHeight+1));

		return true;
	});
}

void ObjCreateHeightmap::SetCamera(int x, int y, int _min, int _max)
{
	if (_min > _max)
		return;

	ObjCamera* cam = (ObjCamera *)GetEngine().FindGameObject("ObjCamera");
	double camHeight = cam->GetTransform().getPos().z;
	cam->setOrthogonalMatrix(0, CONF_Width * CONF_Step, -CONF_Height * CONF_Step, 0, camHeight - _max, camHeight - _min);
	cam->GetTransform().lookat(Vector3(x, y + 240, camHeight), Vector3(x, y + 240, 0), Vector3(0, 1, 0));
}

void ObjCreateHeightmap::CaptureAndSaveHeightMap(int idx)
{
	chars img = jRenderer::GetInst().CaptureBuffer();
	if (img)
	{
		Vector4n cur = mBlocks[idx];
		string posX = jUtils::ToString(cur.x);
		string posY = jUtils::ToString(cur.y);
		string minZ = jUtils::ToString(cur.z);
		string maxZ = jUtils::ToString(cur.w);
		string step = jUtils::ToString(CONF_Step);
		string filename = posX + "_" + posY + "_" + step + "_" + minZ + "_" + maxZ;
		jUtils::MakeFolder(PATH_RESOURCES + string("zmap/"));
		
		Vector3 pos = Vector3(cur.x, cur.y, cur.z);
		Vector3 size = Vector3(CONF_Size, CONF_Size, cur.w - cur.z);
		jRect3D rt(pos, size);

		//jZMapLoader::Save(fullName, rt, CONF_Step, (u32*)&img[0], 640);
		jBitmap::Save(240, 240, 640 * 4, 4, &img[0], PATH_RESOURCES + string("zmap/") + filename + string("_ref.bmp"));
		jBitmap::SaveAlpha(rt, CONF_Step, 640, &img[0], PATH_RESOURCES + string("zmap/") + filename + string(".heights"));
		jBitmap::SaveAlpha(rt, CONF_Step, 640, &img[0], PATH_RESOURCES + string("zmap/") + filename + string(".aces"));
	}
}

bool ObjCreateHeightmap::FindMinMaxHeight(string fullname, Vector2& result)
{
	jParserMeta parse;
	bool ret = parse.Load(fullname);
	_warnif(!ret);

	jMesh mesh(PATH_RESOURCES + string("mesh/") + parse.GetValue(MF_Mesh));
	mesh.Load();
	Vector3 worldPos = parse.GetValue<Vector3>(MF_WorldPos);
	if (mesh.GetVerticies().size() > 0)
	{
		vector<VertexFormat>& vecs = mesh.GetVerticies();
		auto val = minmax_element(vecs.begin(), vecs.end(), [](const VertexFormat& lhs, const VertexFormat& rhs) {
			return lhs.position.z < rhs.position.z;
		});
		result.x = worldPos.z + val.first->position.z;
		result.y = worldPos.z + val.second->position.z;
	}
	else if(!mesh.GetStream().empty())
	{
		string shaderType = parse.GetValue(MF_Shader);
		vector<char>& stream = mesh.GetStream();
		if (shaderType == "terrain")
		{
			VertexFormatPT* pVert = (VertexFormatPT*)&stream[0];
			int vertCnt = stream.size() / sizeof(VertexFormatPT);
			vector<VertexFormatPT*> vecs;
			for (int i = 0; i < vertCnt; ++i)
				vecs.push_back(&pVert[i]);
			auto val = minmax_element(vecs.begin(), vecs.end(), [](const VertexFormatPT* lhs, const VertexFormatPT* rhs) {
				return lhs->p.z < rhs->p.z;
			});
			result.x = worldPos.z + (*val.first)->p.z;
			result.y = worldPos.z + (*val.second)->p.z;
		}
		else if (shaderType == "default")
		{
			VertexFormatPTN* pVert = (VertexFormatPTN*)&stream[0];
			int vertCnt = stream.size() / sizeof(VertexFormatPTN);
			vector<VertexFormatPTN*> vecs;
			for (int i = 0; i < vertCnt; ++i)
				vecs.push_back(&pVert[i]);
			auto val = minmax_element(vecs.begin(), vecs.end(), [](const VertexFormatPTN* lhs, const VertexFormatPTN* rhs) {
				return lhs->p.z < rhs->p.z;
			});
			result.x = worldPos.z + (*val.first)->p.z;
			result.y = worldPos.z + (*val.second)->p.z;
		}
	}

	return true;
}

void jEventHeightMap::OnKeyDown(char ch)
{
	ObjCreateHeightmap* obj = (ObjCreateHeightmap*)GetGameObject();
	if (ch == 'M')
	{
		mIndex = (mIndex + 1) % obj->mBlocks.size();
		Vector4n cur = obj->mBlocks[mIndex];
		double min = cur.z;
		double max = cur.w;
		obj->SetCamera(cur.x, cur.y, min, max);
	}
	else if (ch == 'C')
	{
		obj->CaptureAndSaveHeightMap(mIndex);
	}
}
