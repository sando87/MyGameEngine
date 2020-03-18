#include "jTinyDB.h"

#define TinyDBPath "./db/"

jTinyDB::jTinyDB()
{
}

jTinyDB::~jTinyDB()
{
}

void jTinyDB::DeleteRecord(string tableName, u32 id)
{
	if (mTables.find(tableName) != mTables.end())
		if (mTables[tableName]->records.find(id) != mTables[tableName]->records.end())
			mTables[tableName]->records.erase(id);
}

bool jTinyDB::LoadTable(string tableName)
{
	if (mTables.find(tableName) != mTables.end())
		return true;

	string fullname = TinyDBPath + tableName + ".csv";
	_errorif(!jUtils::ExistFile(fullname));
	strings lines = jUtils::LoadLines(fullname);
	_errorif(lines->size() <= 1);
	strings fields = jUtils::Split2(lines[0], ",");
	int fieldCount = fields->size();
	TinyTable* table = new TinyTable();
	for (int i = 1; i < lines->size(); ++i)
	{
		strings values = jUtils::Split2(lines[i], ",");
		_errorif(values->size() != fieldCount);
		TinyRecord* record = new TinyRecord();
		for (int j = 0; j < fieldCount; ++j)
		{
			string field = fields[j];
			string value = values[j];
			record->data[field] = value;
		}
		u32 id = record->GetValue<u32>("id");
		table->records[id] = record;
	}
	mTables[tableName] = table;

	return true;
}

bool jTinyDB::SaveTable(string tableName)
{
	if(mTables.find(tableName) != mTables.end())
	{
		TinyTable* table = mTables[tableName];
		string ret = table->records.begin()->second->FieldToString() + "\r\n";
		for (auto iterR : table->records)
		{
			TinyRecord* record = iterR.second;
			ret += record->ValueToString() + "\r\n";
		}
		jUtils::SaveToFile(TinyDBPath, tableName + ".csv", ret);
	}

	return true;
}

u32 jTinyDB::WriteRecord(string tableName, u32 recordID, TinyRecord * record)
{
	if (mTables.find(tableName) == mTables.end())
		LoadTable(tableName);

	u32 recID = mTables[tableName]->SetRecord(recordID, record);
	SaveTable(tableName);
	return recID;
}

TinyRecord * jTinyDB::ReadRecord(string tableName, u32 recordID)
{
	if (mTables.find(tableName) == mTables.end())
		LoadTable(tableName);

	TinyTable* table = mTables[tableName];
	return table->records.find(recordID) == table->records.end() ? nullptr : table->records[recordID];
}

u32 TinyTable::SetRecord(u32 id, TinyRecord * record)
{
	u32 recordID = id;
	if (records.find(id) == records.end())
	{
		auto iter = std::max_element(records.begin(), records.end());
		recordID = iter->first + 1;
		record->SetValue("id", recordID);
		records[recordID] = record;
	}
	else
	{
		records[id] = record;
	}
	return recordID;
}

string TinyRecord::FieldToString()
{
	string ret = "";
	for (auto iter : data)
		ret += iter.first + ",";

	return ret;
}

string TinyRecord::ValueToString()
{
	string ret = "";
	for (auto iter : data)
		ret += iter.second + ",";

	return ret;
}

bool DBPlayer::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	name		= record->GetValue<string>("name");
	classes		= record->GetValue<u32>("classes");
	level		= record->GetValue<u32>("level");
	exp			= record->GetValue<u32>("exp");
	statsPA		= record->GetValue<u32>("statsPA");
	statsMA		= record->GetValue<u32>("statsMA");
	statsPD		= record->GetValue<u32>("statsPD");
	statsMD		= record->GetValue<u32>("statsMD");
	statsHP		= record->GetValue<u32>("statsHP");
	statsMP		= record->GetValue<u32>("statsMP");
	statsRemain = record->GetValue<u32>("statsRemain");
	startPos.x = record->GetValue<double>("startPosX");
	startPos.y = record->GetValue<double>("startPosY");
	startPos.z = record->GetValue<double>("startPosZ");
	return true;
}

bool DBPlayer::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	record->SetValue("id",			id);
	record->SetValue("name",		name);
	record->SetValue("classes",		classes);
	record->SetValue("level",		level);
	record->SetValue("exp",			exp);
	record->SetValue("statsPA",		statsPA);
	record->SetValue("statsMA",		statsMA);
	record->SetValue("statsPD",		statsPD);
	record->SetValue("statsMD",		statsMD);
	record->SetValue("statsHP",		statsHP);
	record->SetValue("statsMP",		statsMP);
	record->SetValue("statsRemain", statsRemain);
	record->SetValue("startPosX",	startPos.x);
	record->SetValue("startPosY",	startPos.y);
	record->SetValue("startPosZ",	startPos.z);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

bool DBItemResource::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	mesh		= record->GetValue<string>	("mesh");
	img			= record->GetValue<string>	("img");
	equipPos	= record->GetValue<u32>		("equipPos");
	category	= record->GetValue<string>	("category");
	equipMesh	= record->GetValue<string>	("equipMesh");
	equipImg	= record->GetValue<string>	("equipImg");
	uiImg		= record->GetValue<string>	("uiImg");
	columnNum	= record->GetValue<u32>		("columnNum");
	rowNum		= record->GetValue<u32>		("rowNum");
	uiImgIdx	= record->GetValue<u32>		("uiImgIdx");
	return true;
}

bool DBItemResource::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();
	
	record->SetValue("mesh"		,mesh		);
	record->SetValue("img"		,img		);	
	record->SetValue("equipPos"	,equipPos	);
	record->SetValue("category"	,category	);
	record->SetValue("equipMesh",equipMesh	);
	record->SetValue("equipImg"	,equipImg	);
	record->SetValue("uiImg"	,uiImg		);
	record->SetValue("columnNum",columnNum	);
	record->SetValue("rowNum"	,rowNum		);
	record->SetValue("uiImgIdx"	,uiImgIdx	);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

bool DBItem::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	owner			= record->GetValue<u32>("owner");
	state			= (ItemState)record->GetValue<u32>("state");
	posIndex		= record->GetValue<u32>("posIndex");
	grade			= record->GetValue<u32>("grade");
	pa				= record->GetValue<double>("pa");
	ma				= record->GetValue<double>("ma");
	pd				= record->GetValue<double>("pd");
	md				= record->GetValue<double>("md");
	hp				= record->GetValue<double>("hp");
	mp				= record->GetValue<double>("mp");
	shieldMax		= record->GetValue<double>("shieldMax");
	shieldTime		= record->GetValue<double>("shieldTime");
	spdMove			= record->GetValue<double>("spdMove");
	spdSkill		= record->GetValue<double>("spdSkill");
	cntSkill		= record->GetValue<double>("cntSkill");
	sizeSkill		= record->GetValue<double>("sizeSkill");
	cooltime		= record->GetValue<double>("cooltime");
	proficiency		= record->GetValue<double>("proficiency");
	rsrcID			= record->GetValue<double>("rsrcID");
	return true;
}

bool DBItem::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();
	
	record->SetValue("owner", owner);
	record->SetValue("state", (u32)state);
	record->SetValue("posIndex", posIndex);
	record->SetValue("grade", grade);
	record->SetValue("pa", pa);
	record->SetValue("ma", ma);
	record->SetValue("pd", pd);
	record->SetValue("md", md);
	record->SetValue("hp", hp);
	record->SetValue("mp", mp);
	record->SetValue("shieldMax", shieldMax);
	record->SetValue("shieldTime", shieldTime);
	record->SetValue("spdMove", spdMove);
	record->SetValue("spdSkill", spdSkill);
	record->SetValue("cntSkill", cntSkill);
	record->SetValue("sizeSkill", sizeSkill);
	record->SetValue("cooltime", cooltime);
	record->SetValue("proficiency", proficiency);
	record->SetValue("rsrcID", rsrcID);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

bool DBClasses::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	name		= record->GetValue<string>	("name");
	type		= record->GetValue<u32>		("type");
	bodyMesh	= record->GetValue<string>	("bodyMesh");
	bodyImg		= record->GetValue<string>	("bodyImg");
	bodyAnim	= record->GetValue<string>	("bodyAnim");
	legMesh		= record->GetValue<string>	("legMesh");
	legImg		= record->GetValue<string>	("legImg");
	legAnim		= record->GetValue<string>	("legAnim");
	armMesh		= record->GetValue<string>	("armMesh");
	armImg		= record->GetValue<string>	("armImg");
	armAnim		= record->GetValue<string>	("armAnim");
	footMesh	= record->GetValue<string>	("footMesh");
	footImg		= record->GetValue<string>	("footImg");
	footAnim	= record->GetValue<string>	("footAnim");
	return true;
}

bool DBClasses::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();
	
	record->SetValue("name", name);
	record->SetValue("type", type);
	record->SetValue("bodyMesh", bodyMesh);
	record->SetValue("bodyImg", bodyImg);
	record->SetValue("bodyAnim", bodyAnim);
	record->SetValue("legMesh", legMesh);
	record->SetValue("legImg", legImg);
	record->SetValue("legAnim", legAnim);
	record->SetValue("armMesh", armMesh);
	record->SetValue("armImg", armImg);
	record->SetValue("armAnim", armAnim);
	record->SetValue("footMesh", footMesh);
	record->SetValue("footImg", footImg);
	record->SetValue("footAnim",footAnim);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

void DBInterface::DeleteRecord()
{
	jTinyDB::GetInst().DeleteRecord(tableName, id);
}
