#pragma once
#include "junks.h"

struct ItemInfo{
	string name;
	void* ptr;
	function<void(void*)> releaser;
	clock_t hitTime;
};

class jCaches
{
public:
	static void * CacheClass(string name, function<void*(string)> loader);
	static void * CacheBuffers(string name, function<void*(string)> loader);
	static void * CacheGraphics(string name, function<void*(string)> loader);

private:
	static jCaches * mInst;
	
	jCaches();
	~jCaches();

	unordered_map<string, ItemInfo> mCachedItems;

	void * CacheItem(string name, function<void*(string)> loader, function<void(void*)> releaser);
	void ClearItems(int count);
};

