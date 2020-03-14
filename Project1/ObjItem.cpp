#include "ObjItem.h"
#include "jTrigger.h"
#include "ObjPlayer.h"
#include "jInventory.h"
#include "jParserMeta.h"
#include "jMesh.h"
#include "jImage.h"
#include "jShaderDefault.h"

class jTriggeredByObject : public jTrigger
{
	virtual void OnTriggered(jGameObject* object);
};

ObjItem::ObjItem()
{
}

ObjItem::~ObjItem()
{
}

void ObjItem::LoadProperty(string filename)
{
	mProperty.LoadMetaFile(filename);
}

void ObjItem::OnLoad()
{
	if (mProperty.itemFilename.length() <= 0)
		return;

	AddComponent(new jMesh(PATH_RESOURCES + string("mesh/") + mProperty.meshFilename));
	AddComponent(new jImage(PATH_RESOURCES + string("img/") + mProperty.textureFilename));
	AddComponent(new jShaderDefault());

	AddComponent(new jTriggeredByObject());
}

void jTriggeredByObject::OnTriggered(jGameObject * object)
{
	jInventory* inven = object->FindComponent<jInventory>();
	if (inven == nullptr)
		return;

	inven->PickItem((ObjItem*)GetGameObject());
}

void ItemProperty::LoadMetaFile(string filename)
{
	jParserMeta meta;
	string path = PATH_RESOURCES + string("items/");
	meta.Load(path + filename);

	itemFilename	= meta.GetValue<string>("itemFilename");
	meshFilename	= meta.GetValue<string>("meshFilename");
	textureFilename = meta.GetValue<string>("textureFilename");
	UIImageFilename = meta.GetValue<string>("UIImageFilename");
	UVleft			= meta.GetValue<double>("UVleft");
	UVright			= meta.GetValue<double>("UVright");
	UVtop			= meta.GetValue<double>("UVtop");
	UVbottom		= meta.GetValue<double>("UVbottom");
	state			= (ItemState)meta.GetValue<int>("state");
	category		= (ItemCategory)meta.GetValue<int>("category");
	posIndex		= meta.GetValue<int>("posIndex");
	grade			= meta.GetValue<int>("grade");
	pa				= meta.GetValue<double>("pa");
	ma				= meta.GetValue<double>("ma");
	pd				= meta.GetValue<double>("pd");
	md				= meta.GetValue<double>("md");
	hp				= meta.GetValue<double>("hp");
	mp				= meta.GetValue<double>("mp");
	shieldMax		= meta.GetValue<double>("shieldMax");
	shieldTime		= meta.GetValue<double>("shieldTime");
	spdMove			= meta.GetValue<double>("spdMove");
	spdSkill		= meta.GetValue<double>("spdSkill");
	cntSkill		= meta.GetValue<double>("cntSkill");
	sizeSkill		= meta.GetValue<double>("sizeSkill");
	cooltime		= meta.GetValue<double>("cooltime");
	proficiency		= meta.GetValue<double>("proficiency");
}

void ItemProperty::SaveMetaFile()
{
	jParserMeta meta;
	meta.AddValue("itemFilename", itemFilename);
	meta.AddValue("meshFilename", meshFilename);
	meta.AddValue("textureFilename", textureFilename);
	meta.AddValue("UIImageFilename", UIImageFilename);
	meta.AddValue("UVleft", UVleft);
	meta.AddValue("UVright", UVright);
	meta.AddValue("UVtop", UVtop);
	meta.AddValue("UVbottom", UVbottom);
	meta.AddValue("state", (double)state);
	meta.AddValue("category", (double)category);
	meta.AddValue("posIndex", (double)posIndex);
	meta.AddValue("grade", (double)grade);
	meta.AddValue("pa", pa);
	meta.AddValue("ma", ma);
	meta.AddValue("pd", pd);
	meta.AddValue("md", md);
	meta.AddValue("hp", hp);
	meta.AddValue("mp", mp);
	meta.AddValue("shieldMax", shieldMax);
	meta.AddValue("shieldTime", shieldTime);
	meta.AddValue("spdMove", spdMove);
	meta.AddValue("spdSkill", spdSkill);
	meta.AddValue("cntSkill", cntSkill);
	meta.AddValue("sizeSkill", sizeSkill);
	meta.AddValue("cooltime", cooltime);
	meta.AddValue("proficiency", proficiency);

	string data = meta.ToString();
	string path = PATH_RESOURCES + string("items/");
	jUtils::SaveToFile(path, itemFilename, data);
}
