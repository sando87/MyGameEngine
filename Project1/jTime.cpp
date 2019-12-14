#include "jTime.h"
#include "junks.h"

system_clock::time_point jTime::mStartTime = system_clock::now();
system_clock::time_point jTime::mNowTime = mStartTime;
system_clock::time_point jTime::mPreviousTime = mStartTime;
double jTime::mDeltaTime = 0;
double jTime::mTotalTime = 0;

void jTime::Update()
{
	system_clock::time_point nowTime = system_clock::now();
	duration<double> delta = nowTime - mNowTime;
	mDeltaTime = delta.count();
	duration<double> total = nowTime - mStartTime;
	mTotalTime = total.count();
	mNowTime = nowTime;
}
void jTime::Start()
{
	mPreviousTime = system_clock::now();
}
void jTime::Measure(string printMsg)
{
	system_clock::time_point now = system_clock::now();
	duration<double> delta = now - mPreviousTime;
	mPreviousTime = now;
	_printlog("[%f] %s\n", delta.count(), printMsg.c_str());
}
