#include "jGameObject.h"
#include "jGameObjectMgr.h"
#include "ObjCamera.h"
#include "jComponent.h"
#include "jTransform.h"
#include "jShader.h"
#include "jAnimator.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderSkin.h"
#include "jShaderTerrain.h"
#include "jShaderDefault.h"
#include "jParserMeta.h"
#include "jTypeToString.h"


jGameObject::jGameObject(string name)
{
	mName = name;
	mEngine = &jGameObjectMgr::GetInst();

	mTransform = new jTransform();
	AddComponent(mTransform);
	
}


jGameObject::~jGameObject()
{
	for (jGameObject* child : mChilds)
		delete child;

	if (mParent != nullptr)
		mParent->SubChild(this);
	
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
	{
		jComponent* comp = *iter;
		delete comp;
	}
}

jGameObject * jGameObject::GetParent()
{
	return mParent;
}

jTransform & jGameObject::GetTransform()
{
	return *mTransform;
}

jGameObjectMgr & jGameObject::GetEngine()
{
	return *mEngine;
}
bool jGameObject::LoadTxt(string fullname)
{
	jParserMeta parse;
	bool ret = parse.Load(fullname);
	if (!ret)
	{
		_warn();
		return false;
	}

	mName = parse.GetValue(MF_Name);
	string filenameObj = parse.GetValue(MF_Mesh);
	string filenameAnim = parse.GetValue(MF_Anim);
	vector<string> filenameImgs = parse.GetValues(MF_Img);
	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + filenameObj));
	AddComponent(new jAnimator(PATH_RESOURCES + string("anim/") + filenameAnim));
	for(int i = 0; i < filenameImgs.size(); ++i)
		AddComponent(new jImage(PATH_RESOURCES + string("img/") + filenameImgs[i]));

	string shaderType = parse.GetValue(MF_Shader);
	if (shaderType == "terrain")
	{
		jShaderTerrain* shader = new jShaderTerrain();
		shader->SetAlphaOn(parse.GetValue<bool>(MF_Alpha));
		shader->SetDepthOn(parse.GetValue<bool>(MF_Depth));
		shader->SetRenderOrder(parse.GetValue<double>(MF_Order));
		vector<Vector3> texels = parse.GetValues<Vector3>(MF_Texel);
		ShaderBufferBasic& param = shader->GetParamBasic();
		int cnt = min(sizeof(param.texels) / sizeof(param.texels[0]), texels.size());
		for (int i = 0; i < cnt; ++i)
		{
			param.texels[i].x = texels[i].x;
			param.texels[i].y = texels[i].y;
			param.texels[i].z = texels[i].z;
			param.texels[i].w = 0;
		}
		AddComponent(shader);
	}
	else if (shaderType == "default")
	{
		jShaderDefault* shader = new jShaderDefault();
#ifndef HeightmapCaptureMode
		shader->SetAlphaOn(parse.GetValue<bool>(MF_Alpha)); 
		shader->SetDepthOn(parse.GetValue<bool>(MF_Depth)); 
#else
		shader->SetAlphaOn(false);
		shader->SetDepthOn(true);
#endif
		shader->SetRenderOrder(parse.GetValue<double>(MF_Order));
		AddComponent(shader);
	}
	else if (shaderType == "skin")
	{
		jShaderSkin* shader = new jShaderSkin();
		shader->SetRenderOrder(parse.GetValue<double>(MF_Order));
		AddComponent(shader);
	}

	GetTransform().moveTo(parse.GetValue<Vector3>(MF_WorldPos));
	return true;
}
void jGameObject::AddChild(jGameObject* child)
{
	mChilds.push_back(child);
	child->mParent = this;
}
void jGameObject::SubChild(jGameObject * child)
{
	mChilds.remove(child);
	child->mParent = nullptr;
}
jComponent* jGameObject::AddComponent(jComponent* comp)
{
	if (comp->GetCompName().length() <= 0)
		comp->SetCompName(TypeToString(comp));

	comp->mGameObject = this;
	comp->mEngine = mEngine;
	if (GetEngine().Added(this))
		comp->Load();
	mComponents.push_back(comp);
	return comp;
}
jComponent * jGameObject::FindComponent(string componentName)
{
	for (jComponent* comp : mComponents)
	{
		if (comp->GetCompName() == componentName)
			return comp;
	}
	return nullptr;
}
vector<jComponent*> jGameObject::FindComponents(string componentName)
{
	vector<jComponent*> comps;
	for (jComponent* comp : mComponents)
	{
		if (comp->GetCompName() == componentName)
			comps.push_back(comp);
	}
	return comps;
}
jGameObject * jGameObject::FindGameObject(string objectName)
{
	auto findIter = find_if(mChilds.begin(), mChilds.end(), [&](jGameObject* obj) {
		return obj->GetName() == objectName;
	});
	if (findIter != mChilds.end())
		return *findIter;

	for (auto child : mChilds)
	{
		jGameObject* findObj = child->FindGameObject(objectName);
		if (findObj != nullptr)
			return findObj;
	}
	return nullptr;
}
void jGameObject::Destroy()
{
	mRemove = true;
}
void jGameObject::RemoveComponent(jComponent* comp)
{
	auto iter = find(mComponents.begin(), mComponents.end(), comp);
	if (iter != mComponents.end())
		mComponents.erase(iter);
}

void jGameObject::LoadAll()
{
	OnLoad();
	for (jGameObject* child : mChilds)
		child->LoadAll();
}

void jGameObject::StartAll()
{
	//_echoS(GetName().c_str());
	OnStart();
	for (jGameObject* child : mChilds)
		child->StartAll();
}

void jGameObject::UpdateAll()
{
	OnUpdate();
	for (jGameObject* child : mChilds)
		child->UpdateAll();
}

void jGameObject::GetShaderAll(vector<jShader*>& outs)
{
	vector<jShader*> shaders = FindComponents<jShader>();
	for(jShader* sh : shaders)
		if(sh->GetEnable())
			outs.push_back(sh);

	for (jGameObject* child : mChilds)
		child->GetShaderAll(outs);
}

void jGameObject::LoadComponents()
{
	for (jComponent* comp : mComponents)
	{
		//_echoS(comp->GetCompName().c_str());
		comp->Load();
	}
		

	for (jGameObject* child : mChilds)
		child->LoadComponents();
}

void jGameObject::StartComponents()
{
	for (jComponent* comp : mComponents)
	{
		//_echoS(comp->GetCompName().c_str());
		comp->OnStart();
	}
		

	for (jGameObject* child : mChilds)
		child->StartComponents();
}

void jGameObject::UpdateComponents()
{
	for (jComponent* comp : mComponents)
		if (comp->GetEnable())
			comp->OnUpdate();

	for (jGameObject* child : mChilds)
		child->UpdateComponents();
}
