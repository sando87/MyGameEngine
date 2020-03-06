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

	itemFilename = meta.GetValue<string>("itemFilename");
	meshFilename = meta.GetValue<string>("meshFilename");
	textureFilename = meta.GetValue<string>("textureFilename");
	UIImageFilename = meta.GetValue<string>("UIImageFilename");
	state = (ItemState)meta.GetValue<int>("state");
	category = (ItemCategory)meta.GetValue<int>("category");
	posIndex = meta.GetValue<int>("posIndex");
	pa = meta.GetValue<double>("pa");
	ma = meta.GetValue<double>("ma");
	pd = meta.GetValue<double>("pd");
	md = meta.GetValue<double>("md");
	hp = meta.GetValue<double>("hp");
	mp = meta.GetValue<double>("mp");
	shieldMax	= meta.GetValue<double>("shieldMax");
	shieldTime	= meta.GetValue<double>("shieldTime");
	spdMove		= meta.GetValue<double>("spdMove");
	spdSkill	= meta.GetValue<double>("spdSkill");
	cntSkill	= meta.GetValue<double>("cntSkill");
	sizeSkill	= meta.GetValue<double>("sizeSkill");
	cooltime	= meta.GetValue<double>("cooltime");
	proficiency = meta.GetValue<double>("proficiency");
}

void ItemProperty::SaveMetaFile()
{
	jParserMeta meta;
	meta.SetValue("itemFilename", itemFilename);
	meta.SetValue("meshFilename", meshFilename);
	meta.SetValue("textureFilename", textureFilename);
	meta.SetValue("UIImageFilename", UIImageFilename);
	meta.SetValue("state", (double)state);
	meta.SetValue("category", (double)category);
	meta.SetValue("posIndex", (double)posIndex);
	meta.SetValue("pa", pa);
	meta.SetValue("ma", ma);
	meta.SetValue("pd", pd);
	meta.SetValue("md", md);
	meta.SetValue("hp", hp);
	meta.SetValue("mp", mp);
	meta.SetValue("shieldMax", shieldMax);
	meta.SetValue("shieldTime", shieldTime);
	meta.SetValue("spdMove", spdMove);
	meta.SetValue("spdSkill", spdSkill);
	meta.SetValue("cntSkill", cntSkill);
	meta.SetValue("sizeSkill", sizeSkill);
	meta.SetValue("cooltime", cooltime);
	meta.SetValue("proficiency", proficiency);

	string data = meta.ToString();
	string path = PATH_RESOURCES + string("items/");
	jUtils::SaveToFile(path, itemFilename, data);
}
