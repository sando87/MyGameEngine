#pragma once
#include "junks.h"

enum CoroutineReturn
{
	Stop,
	Keep,
};

enum CoroutineMode
{
	Normal,
	Timer,
	Task,
};

struct CoroutineInfo
{
	bool enabled;
	CoroutineMode mode;
	string name;
	float time_ms;
	float time_back_ms;
	function<CoroutineReturn(void)> coroutine;
	function<void(void)> task;
	bool taskStarted;
	bool taskDone;
	CoroutineInfo() {
		memset(this, 0x00, sizeof(CoroutineInfo));
	}
};

class jCoroutine
{
public:
	jCoroutine();
	~jCoroutine();

	void StartCoroutine(CoroutineInfo coroutineInfo);
	void StopCoroutine(string name);
	void RunCoroutines();

private:
	unordered_map<string, CoroutineInfo> mCoroutines;
};

