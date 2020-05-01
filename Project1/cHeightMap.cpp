#include "cHeightMap.h"
#include "jGameObjectMgr.h"
#include "jTransform.h"
#include "jMesh.h"
#include "jShaderTerrain.h"
#include "ObjTerrainMgr.h"

void cHeightMap::OnStart()
{
	LoadHeightMapFromMesh();
}

void cHeightMap::LoadHeightMapFromMesh()
{
	ObjTerrainMgr* objTerrainMgr = GetEngine().FindGameObject<ObjTerrainMgr>();
	jMesh* mesh = GetGameObject()->FindComponent<jMesh>();
	_exceptif(objTerrainMgr == nullptr, return);
	_exceptif(mesh == nullptr, return);

	Vector3 basePos = GetGameObject()->GetTransform().getPos();
	vector<VertexFormat>& verts = mesh->GetVerticies();
	vector<Vector3> positions;
	for (VertexFormat& vert : verts)
		positions.push_back(basePos + vert.position);

	objTerrainMgr->LoadHeights(positions);
}
