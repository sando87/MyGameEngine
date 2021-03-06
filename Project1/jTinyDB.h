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
	DBInterface(string _tableName) : id(0), tableName(_tableName) {}
	u32 GetID() { return id; }
	string GetTableName() { return tableName; }
	void DeleteRecord();
protected:
	u32 id;
	string tableName;
	virtual bool Load(u32 id) = 0;
	virtual bool Save() = 0;
};
struct DBSpecification : DBInterface
{
	DBSpecification() : DBInterface("specifications") {
		u32 size = sizeof(DBSpecification) - sizeof(DBInterface);
		memset(&pa, 0x00, size);
	}
	double pa;
	double ma;
	double pd;
	double md;
	double hp;
	double mp;
	double shield;
	double MoveSpeed;
	double attackSpeed;
	double castingSpeed;
	double StunRecoverySpeed;
	double HPperSec;
	double MPperSec;
	double shieldResetTime;
	double count;
	double range;
	double keepTime;
	double coolTime;
	double skillMoveSpeed;
	double skillEffectFreq;
	double proficiency;
	double chance;
	double criticalAttack;
	double magicChance;
	virtual bool Load(u32 _id);
	virtual bool Save();
	void operator+=(const DBSpecification& spec);
	void operator-=(const DBSpecification& spec);
	void operator*=(double rate);
	const DBSpecification operator*(const DBSpecification& spec) const;
	const DBSpecification operator/(const DBSpecification& spec) const;
};
struct DBPlayer : DBInterface
{
	DBPlayer() : DBInterface("player") {}
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
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBItemResource : DBInterface
{
	DBItemResource() : DBInterface("itemResources") {}
	string mesh;
	string img;
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
	DBItem() : DBInterface("items") {}
	u32	owner;
	ItemState state;
	u32	posIndex;
	u32	grade;
	u32	rsrcID;
	u32	spec;
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBClasses : DBInterface
{
	DBClasses() : DBInterface("classes") {}
	string	name;
	u32		type;
	string	bodyMesh;
	string	bodyImg;
	string	bodyAnim;
	string	legMesh;
	string	legImg;
	string	legAnim;
	string	armMesh;
	string	armImg;
	string	armAnim;
	string	footMesh;
	string	footImg;
	string	footAnim;
	u32		spec;
	string	skills;
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBSkill : DBInterface
{
	DBSkill() : DBInterface("skills") {}
	string name			;
	string className	;
	string resMesh		;
	string resImg		;
	string resAnim		;
	string resShader	;
	u32 spec;
	virtual bool Load(u32 _id);
	virtual bool Save();
};
struct DBEnemies : DBInterface
{
	DBEnemies() : DBInterface("enemies") {}
	string name;
	u32 level;
	u32 type;
	string resMesh;
	string resImg;
	string resAnim;
	u32 spec;
	u32 skill;
	u32 item;
	virtual bool Load(u32 _id);
	virtual bool Save();
};


class TinyRecord
{
	friend class TinyTable;
	friend class jTinyDB;
public:
	void Clear() { data.clear(); }
	template<typename Ty> Ty GetValue(string field);
	template<typename Ty> void SetValue(string field, Ty value);
private:
	map<string, string> data;
	string FieldToString();
	string ValueToString();
};
class TinyTable
{
	friend class jTinyDB;
public:
	u32 SetRecord(u32 id, TinyRecord* record);
private:
	map<u32, TinyRecord*> records;
};
class jTinyDB
{
public:
	static jTinyDB& GetInst() { static jTinyDB inst; return inst; }

	void DeleteRecord(string tableName, u32 id);
	bool LoadTable(string tableName);
	bool SaveTable(string tableName);

	u32 WriteRecord(string tableName, u32 recordID, TinyRecord* record);
	TinyRecord* ReadRecord(string tableName, u32 recordID);
	template<typename Ty> void ReadRecords(vector<Ty>& records, function<bool(TinyRecord* rec)> comp);

protected:
	jTinyDB();
	~jTinyDB();

	unordered_map<string, TinyTable*> mTables;

};

template<typename Ty>
inline Ty TinyRecord::GetValue(string field)
{
	_warnif(data.find(field) == data.end());
	return data.find(field) == data.end() ? (Ty)0 : (Ty)((u32)jUtils::ToDouble(data[field]));
}
template<>
inline double TinyRecord::GetValue(string field)
{
	_warnif(data.find(field) == data.end());
	return data.find(field) == data.end() ? 0 : jUtils::ToDouble(data[field]);
}
template<>
inline string TinyRecord::GetValue(string field)
{
	_warnif(data.find(field) == data.end());
	return data.find(field) == data.end() ? "null" : data[field];
}

template<typename Ty>
inline void TinyRecord::SetValue(string field, Ty value)
{
	data[field] = jUtils::ToString((u32)value);
}
template<>
inline void TinyRecord::SetValue(string field, double value)
{
	data[field] = jUtils::ToString(value);;
}
template<>
inline void TinyRecord::SetValue(string field, string value)
{
	data[field] = value;
}

template<typename Ty>
inline void jTinyDB::ReadRecords(vector<Ty>& records, function<bool(TinyRecord* rec)> comp)
{
	Ty db;
	string tableName = db.GetTableName();
	if (mTables.find(tableName) == mTables.end())
		LoadTable(tableName);
	
	TinyTable* table = mTables[tableName];
	for (auto elem : table->records)
	{
		if (comp(elem.second))
		{
			u32 id = elem.second->GetValue<u32>("id");
			db.Load(id);
			records.push_back(db);
		}
	}
}
