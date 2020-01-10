#pragma once
#include "junks.h"

struct GraphicResourceInfo
{
	string name;
	void * resource;
	clock_t referTime;
	bool releasable;
};

class jGraphicResources
{
public:
	static jGraphicResources& GetInst() { static jGraphicResources inst; return inst; }

	void * GetResource(string name) {
		if (mResources.find(name) == mResources.end())
			return nullptr;
		mResources[name].referTime = clock();
		return mResources[name].resource;
	}
	void SetResource(string name, void*  res, bool releasable = true) { 
		_warnif(mResources.find(name) != mResources.end());
		GraphicResourceInfo info;
		info.name = name;
		info.releasable = releasable;
		info.referTime = clock();
		info.resource = res;
		mResources[name] = info;
	}
	void * CacheResource(string name, function<void*(string)> _loader)
	{
		if (mResources.find(name) != mResources.end())
		{
			mResources[name].referTime = clock();
			return mResources[name].resource;
		}

		void *res = _loader(name);
		SetResource(name, res);
		return res;
	}
private:
	jGraphicResources() {}
	~jGraphicResources() {}

	unordered_map<string, GraphicResourceInfo> mResources;
};

