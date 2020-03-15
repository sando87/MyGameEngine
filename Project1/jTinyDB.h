#pragma once
#include "junks.h"

class TinyRecord;
class TinyTable;
class jTinyDB;

enum class ItemState {
	Dropped, Carraied, Equipped, Storaged, OnStore, Staged
};
enum class ItemCategory {
	Weapon, Shield, Hat, Ring, Amulet, Shoes, Gloves, Armor, Belt, Pants
};
struct DBInterface
{
public:
	DBInterface() : id(0) {}
	u32 GetID() { return id; }
protected:
	u32 id;
	virtual bool Load(u32 id) = 0;
	virtual bool Save() = 0;
};
struct DBPlayer : DBInterface
{
	string name;
	u32 classes;
	u32 level;
	u32 exp;
	u32 statsPA;
	u32 statsMA;
	u32 statsPD;
	u32 statsMD;
	u32 statsHP;
	u32 statsMP;
	u32 statsRemain;
	Vector3 startPos;
	vector<u32> itemIDs;
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBItemResource : DBInterface
{
	string mesh;
	string img;
	u32 equipPos;
	string category;
	string equipMesh;
	string equipImg;
	string uiImg;
	u32 columnNum;
	u32 rowNum;
	u32 uiImgIdx;
	double Left() const { return (uiImgIdx % columnNum) * (1.0 / columnNum); }
	double Right() const { return ((uiImgIdx % columnNum) + 1) * (1.0 / columnNum); }
	double Top() const { return (uiImgIdx / columnNum) * (1.0 / rowNum); }
	double Bottom() const { return ((uiImgIdx / columnNum) + 1) * (1.0 / rowNum); }
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBItem : DBInterface
{
	ItemState state;
	u32 posIndex;
	u32 grade;
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
	u32 rsrcID;
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBClasses : DBInterface
{
	string name;
	u32 type;
	string bodyMesh;
	string bodyImg;
	string bodyAnim;
	string legMesh;
	string legImg;
	string legAnim;
	string armMesh;
	string armImg;
	string armAnim;
	string footMesh;
	string footImg;
	string footAnim;
	virtual bool Load(u32 _id);
	virtual bool Save();
};


class TinyRecord
{
	friend class TinyTable;
	friend class jTinyDB;
public:
	template<typename Ty> Ty GetValue(string field);
	template<typename Ty> void SetValue(string field, Ty value);
private:
	unordered_map<string, string> data;
	string FieldToString();
	string ValueToString();
};
class TinyTable
{
	friend class jTinyDB;
public:
	void SetRecord(u32 id, TinyRecord* record);
private:
	unordered_map<u32, TinyRecord*> records;
};
class jTinyDB
{
public:
	static jTinyDB& GetInst() { static jTinyDB inst; return inst; }

	bool LoadTable(string tableName);
	bool SaveTableAll();

	void WriteRecord(string tableName, u32 recordID, TinyRecord* record);
	TinyRecord* ReadRecord(string tableName, u32 recordID);

protected:
	jTinyDB();
	~jTinyDB();

	unordered_map<string, TinyTable*> mTables;

};

template<typename Ty>
inline Ty TinyRecord::GetValue(string field)
{
	_warnif(data.find(field) == data.end());
	Ty val = (Ty)jUtils::ToDouble(data[field]);
	return val;
}
template<>
inline string TinyRecord::GetValue(string field)
{
	_warnif(data.find(field) == data.end());
	return data.find(field) == data.end() ? "" : data[field];
}

template<typename Ty>
inline void TinyRecord::SetValue(string field, Ty value)
{
	data[field] = jUtils::ToString(value);
}

template<>
inline void TinyRecord::SetValue(string field, string value)
{
	data[field] = value;
}
