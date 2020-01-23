#include "ObjCreateHeightmap.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderColor.h"
#include "jInput.h"
#include "jRenderer.h"
#include "jBitmap.h"
#include "ObjCamera.h"
#include "jZMapLoader.h"
#include "jParserMeta.h"
#include "jShaderHeader.h"

#define CONF_Width (640)
#define CONF_Height (480)
#define CONF_Step (1)
#define CONF_Size (240)

//HeightMap 생성시 해줘야 할 작업....
//terrain, default  pixel shader 변경 필요
//alpha false depth true => ObjTerrain.cpp Load()함수


ObjCreateHeightmap::ObjCreateHeightmap()
{
}


ObjCreateHeightmap::~ObjCreateHeightmap()
{
}

void ObjCreateHeightmap::OnStart()
{
	LoadBlocksInfo();
	int* _idx = new int();
	jInput::GetInst().mKeyboard += [this, _idx](const unsigned char * info) {
		int& idx = *_idx;
		if (info[31])
		{
			idx = (idx + 1) % mBlocks.size();
			Vector4n cur = mBlocks[idx];
			double min = cur.z;
			double max = cur.w;
			SetCamera(cur.x, cur.y, min, max);
			Sleep(100);
		}
		else if (info[30])
		{
			CaptureAndSaveHeightMap(idx);
			Sleep(100);
		}

	};
	
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
			bool ret = FindMinMaxHeight(foldername + "/" + filename, minmax);
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

	ObjCamera& cam = mEngine->GetCamera();
	double camHeight = cam.GetTransport().getPos().z;
	cam.setOrthogonalMatrix(CONF_Width, CONF_Height, 0, CONF_Width * CONF_Step, -CONF_Height * CONF_Step, 0, camHeight - _max, camHeight - _min);
	cam.GetTransport().lookat(Vector3(x, y + 240, camHeight), Vector3(x, y + 240, 0), Vector3(0, 1, 0));
}

void ObjCreateHeightmap::CaptureAndSaveHeightMap(int idx)
{
	chars img = jRenderer::GetInst().CaptureBuffer();
	if (img)
	{
		Vector4n cur = mBlocks[idx];
		string filename = jUtils::ToString(cur.x) + "_" + jUtils::ToString(cur.y) + ".bmp";
		jUtils::MakeFolder(PATH_RESOURCES + string("zmap/"));
		string fullName = PATH_RESOURCES + string("zmap/") + filename;
		
		Vector3 pos = Vector3(cur.x, cur.y, cur.z);
		Vector3 size = Vector3(CONF_Size, CONF_Size, cur.w - cur.z);
		jRect3D rt(pos, size);

		//jZMapLoader::Save(fullName, rt, CONF_Step, (u32*)&img[0], 640);
		jBitmap::Save(240, 240, 640 * 4, 4, &img[0], fullName.c_str());
		jBitmap::SaveAlpha(rt, CONF_Step, 640, &img[0], fullName + ".alpha");
	}
}

bool ObjCreateHeightmap::FindMinMaxHeight(string path_fullname, Vector2& result)
{
	jParserMeta parse;
	bool ret = parse.Load(path_fullname);
	_warnif(!ret);

	jMesh mesh(PATH_RESOURCES + string("mesh/") + parse.GetValue("mesh"));
	Vector3 worldPos = parse.GetValue<Vector3>("worldPos");
	if (mesh.GetVerticies().size() > 0)
	{
		vector<VertexFormat>& vecs = mesh.GetVerticies();
		auto val = minmax_element(vecs.begin(), vecs.end(), [](const VertexFormat& lhs, const VertexFormat& rhs) {
			return lhs.position.z < rhs.position.z;
		});
		result.x = worldPos.z + val.first->position.z;
		result.y = worldPos.z + val.second->position.z;
	}
	else if(mesh.GetStream())
	{
		string shaderType = parse.GetValue("shader");
		chars stream = mesh.GetStream();
		if (shaderType == "terrain")
		{
			VertexFormatPT* pVert = (VertexFormatPT*)&stream[0];
			int vertCnt = stream->size() / sizeof(VertexFormatPT);
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
			int vertCnt = stream->size() / sizeof(VertexFormatPTN);
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

