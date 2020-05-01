#include "jTinyDB.h"

#define TinyDBPath "./db/"
#define GetDBValue(type, name) name=record->GetValue<type>(#name)
#define SetDBValue(name) record->SetValue(#name, name)

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
	if (!jUtils::ExistFile(fullname))
		return false;

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
	{
		if (!LoadTable(tableName))
		{
			mTables[tableName] = new TinyTable();
		}
	}

	u32 recID = mTables[tableName]->SetRecord(recordID, record);
	SaveTable(tableName);
	return recID;
}

TinyRecord * jTinyDB::ReadRecord(string tableName, u32 recordID)
{
	if (mTables.find(tableName) == mTables.end())
		if (!LoadTable(tableName))
			return nullptr;

	TinyTable* table = mTables[tableName];
	return table->records.find(recordID) == table->records.end() ? nullptr : table->records[recordID];
}

u32 TinyTable::SetRecord(u32 id, TinyRecord * record)
{
	u32 recordID = id;
	if (records.find(id) == records.end())
	{
		auto iter = std::max_element(records.begin(), records.end());
		recordID = iter == records.end() ? 1 : iter->first + 1;
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

void DBInterface::DeleteRecord()
{
	jTinyDB::GetInst().DeleteRecord(tableName, id);
}

bool DBPlayer::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	GetDBValue(string	,name);
	GetDBValue(u32		,classes);
	GetDBValue(u32		,level);
	GetDBValue(u32		,exp);
	GetDBValue(u32		,statsPA);
	GetDBValue(u32		,statsMA);
	GetDBValue(u32		,statsPD);
	GetDBValue(u32		,statsMD);
	GetDBValue(u32		,statsHP);
	GetDBValue(u32		,statsMP);
	GetDBValue(u32		,statsRemain);
	GetDBValue(double	,startPos.x);
	GetDBValue(double	,startPos.y);
	GetDBValue(double	,startPos.z);

	return true;
}

bool DBPlayer::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(name);
	SetDBValue(classes);
	SetDBValue(level);
	SetDBValue(exp);
	SetDBValue(statsPA);
	SetDBValue(statsMA);
	SetDBValue(statsPD);
	SetDBValue(statsMD);
	SetDBValue(statsHP);
	SetDBValue(statsMP);
	SetDBValue(statsRemain);
	SetDBValue(startPos.x);
	SetDBValue(startPos.y);
	SetDBValue(startPos.z);

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

	GetDBValue(string	,mesh);
	GetDBValue(string	,img);
	GetDBValue(string	,category);
	GetDBValue(string	,equipMesh);
	GetDBValue(string	,equipImg);
	GetDBValue(string	,uiImg);
	GetDBValue(u32		,columnNum);
	GetDBValue(u32		,rowNum);
	GetDBValue(u32		,uiImgIdx);
	return true;
}

bool DBItemResource::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(mesh);
	SetDBValue(img);
	SetDBValue(category);
	SetDBValue(equipMesh);
	SetDBValue(equipImg);
	SetDBValue(uiImg);
	SetDBValue(columnNum);
	SetDBValue(rowNum);
	SetDBValue(uiImgIdx);

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

	GetDBValue(u32,			owner	);
	GetDBValue(ItemState,	state	);
	GetDBValue(u32,			posIndex);
	GetDBValue(u32,			grade	);
	GetDBValue(double,		rsrcID	);
	GetDBValue(u32,			spec	);

	return true;
}

bool DBItem::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(owner);
	SetDBValue(state);
	SetDBValue(posIndex);
	SetDBValue(grade);
	SetDBValue(rsrcID);
	SetDBValue(spec);

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
	
	GetDBValue(string,	name);
	GetDBValue(u32,		type);
	GetDBValue(string,	bodyMesh);
	GetDBValue(string,	bodyImg);
	GetDBValue(string,	bodyAnim);
	GetDBValue(string,	legMesh);
	GetDBValue(string,	legImg);
	GetDBValue(string,	legAnim);
	GetDBValue(string,	armMesh);
	GetDBValue(string,	armImg);
	GetDBValue(string,	armAnim);
	GetDBValue(string,	footMesh);
	GetDBValue(string,	footImg);
	GetDBValue(string,	footAnim);
	GetDBValue(u32,		spec);
	GetDBValue(string,	skills);
	return true;
}

bool DBClasses::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(name);
	SetDBValue(type);
	SetDBValue(bodyMesh);
	SetDBValue(bodyImg);
	SetDBValue(bodyAnim);
	SetDBValue(legMesh);
	SetDBValue(legImg);
	SetDBValue(legAnim);
	SetDBValue(armMesh);
	SetDBValue(armImg);
	SetDBValue(armAnim);
	SetDBValue(footMesh);
	SetDBValue(footImg);
	SetDBValue(footAnim);
	SetDBValue(spec);
	SetDBValue(skills);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

bool DBSpecification::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	GetDBValue(double,	pa					);
	GetDBValue(double,	ma					);
	GetDBValue(double,	pd					);
	GetDBValue(double,	md					);
	GetDBValue(double,	hp					);
	GetDBValue(double,	mp					);
	GetDBValue(double,	shield				);
	GetDBValue(double,	MoveSpeed			);
	GetDBValue(double,	attackSpeed			);
	GetDBValue(double,	castingSpeed		);
	GetDBValue(double,	StunRecoverySpeed	);
	GetDBValue(double,	HPperSec			);
	GetDBValue(double,	MPperSec			);
	GetDBValue(double,	shieldResetTime		);
	GetDBValue(double,	count				);
	GetDBValue(double,	range				);
	GetDBValue(double,	keepTime			);
	GetDBValue(double,	coolTime			);
	GetDBValue(double,	skillMoveSpeed		);
	GetDBValue(double,	skillEffectFreq		);
	GetDBValue(double,	proficiency			);
	GetDBValue(double,	chance				);
	GetDBValue(double,	criticalAttack		);
	GetDBValue(double,	magicChance			);

	return true;
}

bool DBSpecification::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();
	
	SetDBValue(pa					);
	SetDBValue(ma					);
	SetDBValue(pd					);
	SetDBValue(md					);
	SetDBValue(hp					);
	SetDBValue(mp					);
	SetDBValue(shield				);
	SetDBValue(MoveSpeed			);
	SetDBValue(attackSpeed			);
	SetDBValue(castingSpeed			);
	SetDBValue(StunRecoverySpeed	);
	SetDBValue(HPperSec				);
	SetDBValue(MPperSec				);
	SetDBValue(shieldResetTime		);
	SetDBValue(count				);
	SetDBValue(range				);
	SetDBValue(keepTime				);
	SetDBValue(coolTime				);
	SetDBValue(skillMoveSpeed		);
	SetDBValue(skillEffectFreq		);
	SetDBValue(proficiency			);
	SetDBValue(chance				);
	SetDBValue(criticalAttack		);
	SetDBValue(magicChance			);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

void DBSpecification::operator+=(const DBSpecification & spec)
{
	pa					+= spec.pa					;
	ma					+= spec.ma					;
	pd					+= spec.pd					;
	md					+= spec.md					;
	hp					+= spec.hp					;
	mp					+= spec.mp					;
	shield				+= spec.shield				;
	MoveSpeed			+= spec.MoveSpeed			;
	attackSpeed			+= spec.attackSpeed			;
	castingSpeed		+= spec.castingSpeed		;
	StunRecoverySpeed	+= spec.StunRecoverySpeed	;
	HPperSec			+= spec.HPperSec			;
	MPperSec			+= spec.MPperSec			;
	shieldResetTime		+= spec.shieldResetTime		;
	count				+= spec.count				;
	range				+= spec.range				;
	keepTime			+= spec.keepTime			;
	coolTime			+= spec.coolTime			;
	skillMoveSpeed		+= spec.skillMoveSpeed		;
	skillEffectFreq		+= spec.skillEffectFreq		;
	proficiency			+= spec.proficiency			;
	chance				+= spec.chance				;
	criticalAttack		+= spec.criticalAttack		;
	magicChance			+= spec.magicChance			;
}

void DBSpecification::operator-=(const DBSpecification & spec)
{
	pa					-= spec.pa					;
	ma					-= spec.ma					;
	pd					-= spec.pd					;
	md					-= spec.md					;
	hp					-= spec.hp					;
	mp					-= spec.mp					;
	shield				-= spec.shield				;
	MoveSpeed			-= spec.MoveSpeed			;
	attackSpeed			-= spec.attackSpeed			;
	castingSpeed		-= spec.castingSpeed		;
	StunRecoverySpeed	-= spec.StunRecoverySpeed	;
	HPperSec			-= spec.HPperSec			;
	MPperSec			-= spec.MPperSec			;
	shieldResetTime		-= spec.shieldResetTime		;
	count				-= spec.count				;
	range				-= spec.range				;
	keepTime			-= spec.keepTime			;
	coolTime			-= spec.coolTime			;
	skillMoveSpeed		-= spec.skillMoveSpeed		;
	skillEffectFreq		-= spec.skillEffectFreq		;
	proficiency			-= spec.proficiency			;
	chance				-= spec.chance				;
	criticalAttack		-= spec.criticalAttack		;
	magicChance			-= spec.magicChance			;
}

void DBSpecification::operator*=(double rate)
{
	pa					*= rate;
	ma					*= rate;
	pd					*= rate;
	md					*= rate;
	hp					*= rate;
	mp					*= rate;
	shield				*= rate;
	MoveSpeed			*= rate;
	attackSpeed			*= rate;
	castingSpeed		*= rate;
	StunRecoverySpeed	*= rate;
	HPperSec			*= rate;
	MPperSec			*= rate;
	shieldResetTime		*= rate;
	count				*= rate;
	range				*= rate;
	keepTime			*= rate;
	coolTime			*= rate;
	skillMoveSpeed		*= rate;
	skillEffectFreq		*= rate;
	proficiency			*= rate;
	chance				*= rate;
	criticalAttack		*= rate;
	magicChance			*= rate;
}

const DBSpecification DBSpecification::operator*(const DBSpecification & spec) const
{
	DBSpecification outSpec;
	outSpec.pa					= pa					*   spec.pa					;
	outSpec.ma					= ma					*   spec.ma					;
	outSpec.pd					= pd					*   spec.pd					;
	outSpec.md					= md					*   spec.md					;
	outSpec.hp					= hp					*   spec.hp					;
	outSpec.mp					= mp					*   spec.mp					;
	outSpec.shield				= shield				*   spec.shield				;
	outSpec.MoveSpeed			= MoveSpeed				*   spec.MoveSpeed			;
	outSpec.attackSpeed			= attackSpeed			*   spec.attackSpeed		;
	outSpec.castingSpeed		= castingSpeed			*   spec.castingSpeed		;
	outSpec.StunRecoverySpeed	= StunRecoverySpeed		*   spec.StunRecoverySpeed	;
	outSpec.HPperSec			= HPperSec				*   spec.HPperSec			;
	outSpec.MPperSec			= MPperSec				*   spec.MPperSec			;
	outSpec.shieldResetTime		= shieldResetTime		*   spec.shieldResetTime	;
	outSpec.count				= count					*   spec.count				;
	outSpec.range				= range					*   spec.range				;
	outSpec.keepTime			= keepTime				*   spec.keepTime			;
	outSpec.coolTime			= coolTime				*   spec.coolTime			;
	outSpec.skillMoveSpeed		= skillMoveSpeed		*   spec.skillMoveSpeed		;
	outSpec.skillEffectFreq		= skillEffectFreq		*   spec.skillEffectFreq	;
	outSpec.proficiency			= proficiency			*   spec.proficiency		;
	outSpec.chance				= chance				*   spec.chance				;
	outSpec.criticalAttack		= criticalAttack		*   spec.criticalAttack		;
	outSpec.magicChance			= magicChance			*   spec.magicChance		;
	return outSpec;
}

const DBSpecification DBSpecification::operator/(const DBSpecification & spec) const
{
	DBSpecification outSpec;
	outSpec.pa					= spec.pa					== 0 ? 1 : (pa					 / spec.pa					);
	outSpec.ma					= spec.ma					== 0 ? 1 : (ma					 / spec.ma					);
	outSpec.pd					= spec.pd					== 0 ? 1 : (pd					 / spec.pd					);
	outSpec.md					= spec.md					== 0 ? 1 : (md					 / spec.md					);
	outSpec.hp					= spec.hp					== 0 ? 1 : (hp					 / spec.hp					);
	outSpec.mp					= spec.mp					== 0 ? 1 : (mp					 / spec.mp					);
	outSpec.shield				= spec.shield				== 0 ? 1 : (shield				 / spec.shield				);
	outSpec.MoveSpeed			= spec.MoveSpeed			== 0 ? 1 : (MoveSpeed			 / spec.MoveSpeed			);
	outSpec.attackSpeed			= spec.attackSpeed			== 0 ? 1 : (attackSpeed			 / spec.attackSpeed			);
	outSpec.castingSpeed		= spec.castingSpeed			== 0 ? 1 : (castingSpeed		 / spec.castingSpeed		);
	outSpec.StunRecoverySpeed	= spec.StunRecoverySpeed	== 0 ? 1 : (StunRecoverySpeed	 / spec.StunRecoverySpeed	);
	outSpec.HPperSec			= spec.HPperSec				== 0 ? 1 : (HPperSec			 / spec.HPperSec			);
	outSpec.MPperSec			= spec.MPperSec				== 0 ? 1 : (MPperSec			 / spec.MPperSec			);
	outSpec.shieldResetTime		= spec.shieldResetTime		== 0 ? 1 : (shieldResetTime		 / spec.shieldResetTime		);
	outSpec.count				= spec.count				== 0 ? 1 : (count				 / spec.count				);
	outSpec.range				= spec.range				== 0 ? 1 : (range				 / spec.range				);
	outSpec.keepTime			= spec.keepTime				== 0 ? 1 : (keepTime			 / spec.keepTime			);
	outSpec.coolTime			= spec.coolTime				== 0 ? 1 : (coolTime			 / spec.coolTime			);
	outSpec.skillMoveSpeed		= spec.skillMoveSpeed		== 0 ? 1 : (skillMoveSpeed		 / spec.skillMoveSpeed		);
	outSpec.skillEffectFreq		= spec.skillEffectFreq		== 0 ? 1 : (skillEffectFreq		 / spec.skillEffectFreq		);
	outSpec.proficiency			= spec.proficiency			== 0 ? 1 : (proficiency			 / spec.proficiency			);
	outSpec.chance				= spec.chance				== 0 ? 1 : (chance				 / spec.chance				);
	outSpec.criticalAttack		= spec.criticalAttack		== 0 ? 1 : (criticalAttack		 / spec.criticalAttack		);
	outSpec.magicChance			= spec.magicChance			== 0 ? 1 : (magicChance			 / spec.magicChance			);
	return outSpec;
}

bool DBSkill::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	GetDBValue(string,	name		);
	GetDBValue(string,	className	);
	GetDBValue(string,	resMesh		);
	GetDBValue(string,	resImg		);
	GetDBValue(string,	resAnim		);
	GetDBValue(string,	resShader	);
	GetDBValue(u32,		spec		);

	return true;
}

bool DBSkill::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(name			);
	SetDBValue(className	);
	SetDBValue(resMesh		);
	SetDBValue(resImg		);
	SetDBValue(resAnim		);
	SetDBValue(resShader	);
	SetDBValue(spec			);

	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}

bool DBEnemies::Load(u32 _id)
{
	id = _id;
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	_warnif(record == nullptr);
	if (record == nullptr)
		return false;

	GetDBValue(string,	name	);
	GetDBValue(u32,		level	);
	GetDBValue(u32,		type	);
	GetDBValue(string,	resMesh	);
	GetDBValue(string,	resImg	);
	GetDBValue(string,	resAnim	);
	GetDBValue(u32,		spec	);
	GetDBValue(u32,		skill	);
	GetDBValue(u32,		item	);

	return true;
}

bool DBEnemies::Save()
{
	TinyRecord *record = jTinyDB::GetInst().ReadRecord(tableName, id);
	if (record == nullptr)
		record = new TinyRecord();

	SetDBValue(name		);
	SetDBValue(level	);
	SetDBValue(type		);
	SetDBValue(resMesh	);
	SetDBValue(resImg	);
	SetDBValue(resAnim	);
	SetDBValue(spec		);
	SetDBValue(skill	);
	SetDBValue(item		);
	
	id = jTinyDB::GetInst().WriteRecord(tableName, id, record);
	return true;
}
