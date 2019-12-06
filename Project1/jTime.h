#pragma once

#include <chrono>

static class jTime
{
public:
	jTime();
	~jTime();

private:
	static std::chrono::system_clock::time_point mStartTime;
	static std::chrono::system_clock::time_point mNowTime;
	static double mDeltaTime;
	static double mTotalTime;

public:
	static void Update();
	static double Delta() { return mDeltaTime; }
	static double Total() { return mTotalTime; }
};

