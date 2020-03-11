#include "jTinyDB.h"

#define TinyDBPath "./db/"

jTinyDB::jTinyDB()
{
}

jTinyDB::~jTinyDB()
{
	SaveTableAll();
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
		u32 id = jUtils::ToDouble(values[0]);
		table->records[id] = record;
	}
	mTables[tableName] = table;

	return true;
}

bool jTinyDB::SaveTableAll()
{
	
	for (auto iterT : mTables)
	{
		string tableName = iterT.first;
		TinyTable* table = iterT.second;
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

TinyRecord * jTinyDB::ReadRecord(string tableName, u32 recordID)
{
	if (mTables.find(tableName) == mTables.end())
		return nullptr;

	TinyTable* table = mTables[tableName];
	if (table->records.find(recordID) == table->records.end())
		return nullptr;

	return table->records[recordID];
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
