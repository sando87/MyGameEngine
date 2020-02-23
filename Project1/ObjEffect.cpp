#include "ObjEffect.h"
#include "jShaderSprite.h"
#include "jMesh.h"
#include "jImage.h"
#include "jTransform.h"
#include "ObjCamera.h"

ObjEffect::ObjEffect()
{
}


ObjEffect::~ObjEffect()
{
}

void ObjEffect::OnLoad()
{
	jMesh* mesh = new jMesh();
	mesh->LoadRectangle(Vector2(), Vector2(5, 5));
	vector<VertexFormat>& verticies = mesh->GetVerticies();
	verticies[0].texel = Vector2f(0, 1);
	verticies[1].texel = Vector2f(1, 1);
	verticies[2].texel = Vector2f(0, 0);
	verticies[3].texel = Vector2f(1, 0);
	//verticies[0].normal = Vector3f(1, 1, 1);
	//verticies[1].normal = Vector3f(1, 1, 1);
	//verticies[2].normal = Vector3f(1, 1, 1);
	//verticies[3].normal = Vector3f(1, 1, 1);
	AddComponent(mesh);

	jImage* imgae = new jImage("./res/img/sprite.tga");
	AddComponent(imgae);

	mShader = new jShaderSprite();
	ShaderBufferBasic& param = mShader->GetParamBasic();
	param.spriteStep = Vector2f(1.0f, 1.0f);
	param.spriteIndex = Vector2f(idx, 0);
	mShader->SetAlphaOn(true);
	mShader->SetDepthOn(false);
	AddComponent(mShader);

}

void ObjEffect::OnStart()
{
	ObjCamera* cam = jGameObjectMgr::GetInst().FindGameObject<ObjCamera>();
	GetTransform().lookat(Vector3(), cam->GetTransform().getView(), Vector3(0, 0, 1));
}

void ObjEffect::OnUpdate()
{
	//idx = (idx+1) % 500;
	//
	//ShaderBufferBasic& param = mShader->GetParamBasic();
	//param.spriteIndex = Vector2f((int)(idx/100), 0);
}
