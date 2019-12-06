#include "jTime.h"


std::chrono::system_clock::time_point jTime::mStartTime;
std::chrono::system_clock::time_point jTime::mNowTime;
double jTime::mDeltaTime = 0;
double jTime::mTotalTime = 0;

jTime::jTime()
{
	mStartTime = std::chrono::system_clock::now();
	mNowTime = mStartTime;
	mDeltaTime = 0;
	mTotalTime = 0;
}


jTime::~jTime()
{
}

void jTime::Update()
{
	std::chrono::system_clock::time_point nowTime = std::chrono::system_clock::now();
	std::chrono::duration<double> delta = nowTime - mNowTime;
	mDeltaTime = delta.count();
	std::chrono::duration<double> total = nowTime - mStartTime;
	mTotalTime = total.count();
	mNowTime = nowTime;
}