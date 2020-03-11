#pragma once
#include "junks.h"

struct TinyRecord
{
	unordered_map<string, string> data;
	string FieldToString();
	string ValueToString();
};
struct TinyTable
{
	unordered_map<u32, TinyRecord*> records;
};
class jTinyDB
{
public:
	static jTinyDB& GetInst() { static jTinyDB inst; return inst; }

	bool LoadTable(string tableName);
	bool SaveTableAll();

	TinyRecord* ReadRecord(string tableName, u32 recordID);

protected:
	jTinyDB();
	~jTinyDB();

	unordered_map<string, TinyTable*> mTables;

};

