#pragma once
#include "jComponent.h"
#include "junks.h"
#include "jRect.h"

struct CrashCapsule
{
	Vector3 center;
	double round;
	double height;
};

class jCrash;
typedef shared_ptr_array<jCrash*> jCrashs;
class jCrash :
	public jComponent
{
public:
	jCrash();
	virtual ~jCrash();

	void Init(CrashCapsule shape, function<void(jCrashs)> _onCrash)
	{
		mShape = shape;
		mCallback = _onCrash;
	}
	bool Crashed(jCrash* target);
	void OnCrash();
	jRect GetRect();
	void AddCrashed(jCrash* target) { mCrashs->push_back(target); }
	const CrashCapsule GetShape() { return mShape; }

private:
	CrashCapsule mShape;
	jCrashs mCrashs;
	function<void(jCrashs)> mCallback;
};

