#pragma once
#include "junks.h"

enum CorCmd
{
	Stop,
	Keep,
};

enum CorMode
{
	Normal,
	Timer,
	Task,
};

struct CorMember
{
	int intVal;
	string stringVal;
	double doubleVal;
	void* ptr;
	CorMember() {
		intVal = 0; stringVal = ""; doubleVal = 0; ptr = nullptr;
	}
};

struct CoroutineInfo
{
	bool enabled;
	CorMode mode;
	string name;
	CorMember userData;
	float time_ms;
	float time_back_ms;
	function<CorCmd(CorMember&, bool)> coroutine;
	function<void(void)> task;
	bool taskStarted;
	bool taskDone;
	bool firstCalled;
	thread *pThread;
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

