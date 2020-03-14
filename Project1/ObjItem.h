#pragma once
#include "jGameObject.h"

enum class ItemState{
	Dropped, Carraied, Equipped, Storaged, OnStore, Staged
};
enum class ItemCategory {
	Weapon, Shield, Hat, Ring, Amulet, Shoes, Gloves, Armor, Belt, Pants
};
struct ItemProperty
{
	string itemFilename;
	string meshFilename;
	string textureFilename;
	string UIImageFilename;
	double UVleft;
	double UVright;
	double UVtop;
	double UVbottom;
	ItemState state;
	ItemCategory category;
	int posIndex;
	int grade;
	double pa;
	double ma;
	double pd;
	double md;
	double hp;
	double mp;
	double shieldMax;
	double shieldTime;
	double spdMove;
	double spdSkill;
	double cntSkill;
	double sizeSkill;
	double cooltime;
	double proficiency;
	void LoadMetaFile(string filename);
	void SaveMetaFile();
};

class ObjItem :
	public jGameObject
{
public:
	ObjItem();
	~ObjItem();

	void LoadProperty(string filename);
	const ItemProperty& GetProperty() { return mProperty; }
	void SetPropPosIdx(int idx) { mProperty.posIndex = idx; }

protected:
	virtual void OnLoad();

	ItemProperty mProperty;
};

