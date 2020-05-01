#include "oCollisionMgr.h"
#include "jMesh.h"
#include "jShaderColor.h"
#include "ObjCamera.h"
#include "oUserInputDriver.h"

class jShaderColliderBoxes : public jShaderColor
{
public:
	jShaderColliderBoxes() {}
	virtual ~jShaderColliderBoxes() {}

	virtual void OnLoad();
	virtual bool OnRender(ObjCamera* cam);
protected:
	vector<cCollider*> mColliders;
	oUserInputDriver* mObjMouseInfo;

	void SetConfig(ObjCamera* cam);
	void SetBoxTranform(ObjCamera* cam, Vector3 centerPos, Vector3 size, Vector4 color);
	void DrawColliderBox();
};

void oCollisionMgr::AddStaticCollider(jRectangle2D worldRect, cCollider* collider)
{
	vector<u64> keys;
	GetKeys(worldRect, keys);
	for (u64 key : keys)
	{
		if (mGridsStatic.find(key) == mGridsStatic.end())
			mGridsStatic[key] = CollisionGrid();

		mGridsStatic[key].key = key;
		mGridsStatic[key].colliders[collider] = collider;
	}
}

void oCollisionMgr::AddStaticCollider(Vector2 pos, cCollider * collider)
{
	u64 key = ToKey(pos);
	if (mGridsStatic.find(key) == mGridsStatic.end())
		mGridsStatic[key] = CollisionGrid();

	mGridsStatic[key].key = key;
	mGridsStatic[key].colliders[collider] = collider;
}

void oCollisionMgr::AddDynamicCollider(cCollider* collider)
{
	mDynamicColliders.push_back(collider);
}

void oCollisionMgr::SubDynamicCollider(cCollider * collider)
{
	mDynamicColliders.remove(collider);
	for (auto& item : mGridsDynamic)
		item.second.colliders.erase(collider);
}

CrashResult oCollisionMgr::RayCast(Vector3 startPos, Vector3 endPos)
{
	vector<u64> keys;
	vector<cCollider*> colliders;
	GetKeys(startPos, endPos, keys);
	GetColliders(keys, colliders);
	for (cCollider* col : colliders)
	{
		if (false == col->GetEnableCollision())
			continue;

		jShape* shape = col->GetShape();
		jShapeLine line;
		line.PositionStart = startPos;
		line.PositionEnd = endPos;
		CrashResult ret = shape->IsCrash(&line);
		if(ret.isCrash)
			return ret;
	}
	return CrashResult();
}

cCollider* oCollisionMgr::CrashedCollider(Vector3 pos)
{
	u64 key = ToKey(pos);
	vector<cCollider*> colliders;
	GetColliders(key, colliders);
	cCollider* closestCollider = nullptr;
	double minDistance = DBL_MAX;
	for (cCollider* collider : colliders)
	{
		if (false == collider->GetEnableCollision())
			continue;

		jShape* shape = collider->GetShape();
		jShapeSphere sphere;
		sphere.Position = pos;
		sphere.Round = 0.1;
		CrashResult ret = shape->IsCrash(&sphere);
		double dist = Vector2(shape->GetBox().Center()).distance(Vector2(pos));
		if (ret.isCrash && dist < minDistance)
		{
			closestCollider = collider;
			minDistance = dist;
		}
	}

	return closestCollider;
}

void oCollisionMgr::GetColliders(u64 key, vector<cCollider*>& colliders)
{
	if (mGridsStatic.find(key) != mGridsStatic.end())
	{
		for (auto collider : mGridsStatic[key].colliders)
			colliders.push_back(collider.first);
	}

	if (mGridsDynamic.find(key) != mGridsDynamic.end())
	{
		for (auto collider : mGridsDynamic[key].colliders)
			colliders.push_back(collider.first);
	}
}

void oCollisionMgr::GetColliders(vector<u64>& keys, vector<cCollider*>& colliders)
{
	unordered_map<cCollider*, int> checkSameCollider;
	for (u64 key : keys)
	{
		vector<cCollider*> collidersSub;
		GetColliders(key, collidersSub);
		for (cCollider* col : collidersSub)
		{
			if (checkSameCollider.find(col) == checkSameCollider.end())
			{
				checkSameCollider[col] = 1;
				colliders.push_back(col);
			}
		}
	}
}

void oCollisionMgr::GetColliders(jRectangle2D rect, vector<cCollider*>& colliders)
{
	vector<u64> keys;
	GetKeys(rect, keys);
	GetColliders(keys, colliders);
}

void oCollisionMgr::GetKeys(jRectangle2D rect, vector<u64>& keys)
{
	vector<Vector2> points;
	rect.ClipGrid(mStep, points);
	for (Vector2& point : points)
		keys.push_back(ToKey(point));
}

void oCollisionMgr::GetKeys(Vector3 startPos, Vector3 endPos, vector<u64>& keys)
{
	jLine3D line(startPos, endPos);
	vector<Vector3> points;
	line.ClipGrid(mStep, points);
	for(Vector3 pt : points)
		keys.push_back(ToKey(pt));
}

void oCollisionMgr::OnLoad()
{
	jMesh* mesh = new jMesh();
	mesh->LoadCubeOutline(1);
	AddComponent(mesh);

	AddComponent(new jShaderColliderBoxes());
}

void oCollisionMgr::OnUpdate()
{
	UpdateDynamicGrid();

	unordered_map<u64, CollisionInfo> pairs;
	CandidatePairs(pairs);
	
	InvokeColliderEvent(pairs);
}

void oCollisionMgr::InvokeColliderEvent(unordered_map<u64, CollisionInfo>& pairs)
{
	for (auto pair : pairs)
	{
		cCollider* left = pair.second.left;
		cCollider* right = pair.second.right;
		CrashResult ret = left->GetShape()->IsCrash(right->GetShape());
		if (ret.isCrash)
		{
			left->InvokeCollision(right, ret);
			right->InvokeCollision(left, ret);
		}
	}
}

void oCollisionMgr::CandidatePairs(unordered_map<u64, CollisionInfo>& pairs)
{
	for (auto grid : mGridsDynamic)
	{
		u64 key = grid.first;
		vector<cCollider*> colliders;
		GetColliders(key, colliders);
		for (auto iterOut = colliders.begin(); iterOut != colliders.end(); ++iterOut)
		{
			if (false == (*iterOut)->GetEnableCollision())
				continue;

			for (auto iterIn = std::next(iterOut); iterIn != colliders.end(); ++iterIn)
			{
				if (false == (*iterIn)->GetEnableCollision())
					continue;

				CollisionInfo info;
				info.left = *iterOut;
				info.right = *iterIn;
				info.key = key;
				u64 pairKey = (((u64)info.left << 32) | ((u64)info.right & 0xffffffffULL));
				pairs[pairKey] = info;
			}
		}
	}
}

void oCollisionMgr::UpdateDynamicGrid()
{
	mGridsDynamic.clear();
	for (cCollider* col : mDynamicColliders)
	{
		jRectangle2D rect = col->GetShape()->GetBox().Top();
		vector<u64> keys;
		if (rect.GetSize().length() < mStep)
			keys.push_back(ToKey(rect.GetCenter()));
		else
			GetKeys(rect, keys);

		for (u64 key : keys)
		{
			if (mGridsDynamic.find(key) == mGridsDynamic.end())
				mGridsDynamic[key] = CollisionGrid();

			mGridsDynamic[key].key = key;
			mGridsDynamic[key].colliders[col] = col;
		}
	}
}

void jShaderColliderBoxes::OnLoad()
{
	jShaderColor::OnLoad();
	mObjMouseInfo = GetEngine().FindGameObject<oUserInputDriver>();
}

bool jShaderColliderBoxes::OnRender(ObjCamera * cam)
{
	//SetConfig(cam);
	//
	//oCollisionMgr* obj = (oCollisionMgr*)GetGameObject();
	//Vector3 pos = mObjMouseInfo->GetPositionOnGround();
	//double round = 10;
	//double step = 3.0;
	//jRectangle2D rect(pos.x - round, pos.y - round, round * 2, round * 2);
	//vector<Vector2> points;
	//rect.ClipGrid(step, points);
	//for (Vector2 point : points)
	//{
	//	u64 key = obj->ToKey(point);
	//	if (obj->mGridsStatic.find(key) == obj->mGridsStatic.end())
	//		continue;
	//
	//	CollisionGrid& grid = obj->mGridsStatic[key];
	//	for (cCollider* col : grid.colliders)
	//	{
	//		jShapeGridCylinder* shape = (jShapeGridCylinder*)col->GetShape();
	//		if (shape->Shapes.IsContains(key))
	//		{
	//			jShapeCylinder& cylinder = shape->Shapes[key];
	//			jRect3D box = cylinder.GetBox();
	//			SetBoxTranform(cam, box.Center(), box.Size(), ((cColliderGridPole*)col)->Hovered ? Vector4(0,0,0,1) : Vector4(1,1,1,1));
	//			DrawColliderBox();
	//		}
	//	}
	//}

	//if (mColliders.empty())
	//{
	//	jRectangle2D rect = cam->GetGroundRect();
	//	vector<cCollider*> colliders;
	//	((oCollisionMgr*)GetGameObject())->GetColliders(rect, colliders);
	//	for (cCollider* col : colliders)
	//		mColliders.push_back(col);
	//}
	//
	//SetConfig(cam);
	//for (cCollider* col : mColliders)
	//{
	//	jRect3D box = col->GetShape()->GetBox();
	//	SetBoxTranform(cam, box.Center(), box.Size(), Vector4(1,1,1,1));
	//	DrawColliderBox();
	//}
	return true;
}

void jShaderColliderBoxes::SetConfig(ObjCamera* cam)
{
	SetRenderContext(cam);

	D3D_PRIMITIVE_TOPOLOGY prim;
	if (mMesh->GetPrimitive() == PrimitiveMode::LineList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleList)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	else if (mMesh->GetPrimitive() == PrimitiveMode::TriangleStrip)
		prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	u32 offset = 0;
	u32 stride = sizeof(VertexFormatPC);
	mDevContext->IASetVertexBuffers(0, 1, &mCBVertex, &stride, &offset);

	mDevContext->IASetPrimitiveTopology(prim);

	mDevContext->IASetIndexBuffer(mCBIndex, DXGI_FORMAT_R32_UINT, 0);
}

void jShaderColliderBoxes::SetBoxTranform(ObjCamera* cam, Vector3 centerPos, Vector3 size, Vector4 color)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(mDevContext->Map(mCBBasic, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		_warn();

	Matrix4f mat;
	mat.identity();
	mat[0] = size.x;
	mat[5] = size.y;
	mat[10] = size.z;
	mat[12] = centerPos.x;
	mat[13] = centerPos.y;
	mat[14] = centerPos.z;
	mat.transpose();

	ShaderBufferBasic* dataPtr = (ShaderBufferBasic*)mappedResource.pData;
	dataPtr->worldMat = mat;
	dataPtr->viewMat = cam->getPosMat_D3D().transpose();
	dataPtr->projectionMat = cam->getProjMat().transpose();
	dataPtr->matDiffuse = Vector4f(color.x, color.y, color.z, color.w);
	mDevContext->Unmap(mCBBasic, 0);
}

void jShaderColliderBoxes::DrawColliderBox()
{
	int indiCount = mMesh->GetIndicies().size();
	mDevContext->DrawIndexed(indiCount, 0, 0);
}
