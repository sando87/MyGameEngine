#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRect3D.h"

class jCrash;
typedef shared_ptr_array<jCrash*> jCrashs;

struct CrashCapsule
{
	double round;
	double height;
};
struct CrashTrigInfo
{
	bool isNew;
	bool isKeep;
	jCrash* crash;
};

class jCrash :
	public jComponent
{
public:
	jCrash();
	virtual ~jCrash();

	jCrash* Init(double _r, double _h, function<void(jCrashs)> _onCrash)
	{
		mShape.round = _r;
		mShape.height = _h;
		mCallbacksEnter = _onCrash;
		return this;
	}
	bool Crashed(jCrash* target);
	void CallbackCrash();
	jRect3D GetRect();
	void AddCrashed(jCrash* target);
	const CrashCapsule GetShape() { return mShape; }

private:
	CrashCapsule mShape;
	unordered_map<jCrash*, CrashTrigInfo> mCrashes;
	function<void(jCrashs)> mCallbacksEnter;
	function<void(jCrashs)> mCallbacksKeep;
	function<void(jCrashs)> mCallbacksLeave;
};

