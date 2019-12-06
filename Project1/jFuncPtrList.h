#pragma once

#include <list>
#include <functional>
using namespace std;

template<class T>
class jFuncPtrList
{
public:
	jFuncPtrList() { mpCurFunc = nullptr; }
	~jFuncPtrList() {}

	list<function<T>>& GetList()
	{
		return mList;
	}

	template<class... ArgTypes>
	void operator()(ArgTypes... args)
	{
		for (auto iter = mList.begin(); iter != mList.end(); ++iter)
		{
			function<T> func = *iter;
			func(args...);
		}
	}
	void operator+=(function<T> _func)
	{
		mList.push_back(_func);
	}
	void operator-=(function<T> _func)
	{
		for (auto iter = mList.begin(); iter != mList.end(); ++iter)
		{
			if (*iter == _func)
			{
				mList.erase(iter);
				break;
			}
		}
	}
	void operator=(function<T> _func)
	{
		mList.clear();
		mList.push_back(_func);
	}

private:
	list<function<T>> mList;
	function<T> *mpCurFunc;
};

