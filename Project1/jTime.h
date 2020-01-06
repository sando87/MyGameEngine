#pragma once

#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

class jTime
{
private:
	static system_clock::time_point mStartTime;
	static system_clock::time_point mNowTime;
	static double mDeltaTime;
	static double mTotalTime;
	static system_clock::time_point mPreviousTime;

public:
	static void Update();
	static double Now();
	static double Delta() { return mDeltaTime; }
	static double Total() { return mTotalTime; }
	static void Start();
	static void Measure(string printMsg);
};

