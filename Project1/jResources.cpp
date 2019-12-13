#include "jResources.h"
#include "jDirectXheader.h"

#define CacheSize 100

jResources * jResources::mInst = new jResources();
void * jResources::CacheClass(string name, function<void*(string)> loader)
{
	return mInst->CacheItem(name, loader, [](void* ptr) { delete ptr; });
}
void * jResources::CacheBuffers(string name, function<void*(string)> loader)
{
	return mInst->CacheItem(name, loader, [](void* ptr) { delete[] ptr; });
}
void * jResources::CacheGraphics(string name, function<void*(string)> loader)
{
	return mInst->CacheItem(name, loader, [](void* ptr) { ((IUnknown*)ptr)->Release(); });
}

jResources::jResources()
{
}
jResources::~jResources()
{
	ClearItems(mCachedItems.size());
}
void jResources::ClearItems(int count)
{
	vector<ItemInfo> items;
	for (auto item : mCachedItems)
		items.push_back(item.second);

	sort(items.begin(), items.end(), [](ItemInfo& left, ItemInfo& right) {
		return left.hitTime < right.hitTime;
	});

	int num = min(items.size(), count);
	for (int i = 0; i < num; ++i)
	{
		items[i].releaser(items[i].ptr);
		mCachedItems.erase(items[i].name);
	}
}
void * jResources::CacheItem(string name, function<void*(string)> loader, function<void(void*)> releaser)
{
	if (mCachedItems.find(name) == mCachedItems.end())
	{
		if (mCachedItems.size() >= CacheSize)
			ClearItems(CacheSize / 2);

		void *ptr = loader(name);
		if (ptr == nullptr)
			return nullptr;

		ItemInfo info;
		info.name = name;
		info.ptr = ptr;
		info.releaser = releaser;
		info.hitTime = clock();
		mCachedItems[name] = info;
	}

	mCachedItems[name].hitTime = clock();
	return mCachedItems[name].ptr;
}