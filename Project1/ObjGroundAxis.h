#pragma once
#include "jGameObject.h"
#include "Matrices.h"

class jModel;
class jShaderColor;

class ObjGroundAxis :
	public jGameObject
{
public:
	ObjGroundAxis();
	virtual ~ObjGroundAxis();

private:
	void OnStart();
	void OnUpdate();
	void OnDraw();

	jModel* mModel;
	jModel* mModelX;
	jShaderColor * mShader;
	Matrix4 mWorldMat;
};

