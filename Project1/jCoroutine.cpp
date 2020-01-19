#include "jCoroutine.h"
#include "jTime.h"


jCoroutine::jCoroutine()
{
}


jCoroutine::~jCoroutine()
{
}

void jCoroutine::StartCoroutine(CoroutineInfo coroutineInfo)
{
	if (coroutineInfo.name.length() <= 0)
	{
		static u32 index = 0;
		index++;
		coroutineInfo.name = "__AutoCoroutineName__" + jUtils::ToString(index) + "__";
	}

	mCoroutines[coroutineInfo.name] = coroutineInfo;
}
void jCoroutine::StopCoroutine(string name)
{
	if(mCoroutines.find(name) != mCoroutines.end())
		mCoroutines[name].enabled = false;
}

void jCoroutine::RunCoroutines()
{
	for (auto iter = mCoroutines.begin(); iter != mCoroutines.end();)
	{
		CoroutineInfo& cInfo = iter->second;
		if (cInfo.enabled == false && cInfo.pThread == nullptr)
		{
			mCoroutines.erase(iter++);
			continue;
		}

		if (cInfo.mode == CorMode::Normal) //코루틴 호출 후 false반환시 종료
		{
			cInfo.enabled = cInfo.coroutine(cInfo.userData, cInfo.firstCalled) == CorCmd::Keep ? true : false;
			cInfo.firstCalled = false;
		}
		else if (cInfo.mode == CorMode::Timer) //일정시간 지연 후 코루틴 호출
		{
			cInfo.time_ms -= jTime::Delta() * 1000.0;
			if (cInfo.time_ms <= 0)
			{
				cInfo.enabled = cInfo.coroutine(cInfo.userData, cInfo.firstCalled) == CorCmd::Keep ? true : false;
				cInfo.time_ms = cInfo.time_back_ms;
				cInfo.firstCalled = false;
			}
		}
		else if (cInfo.mode == CorMode::Task) //쓰레드 완료 후 코루틴 호출
		{
			if (cInfo.taskStarted == false)
			{
				cInfo.taskStarted = true;
				cInfo.taskDone = false;
				cInfo.pThread = new thread([&cInfo]() {
					cInfo.task();
					cInfo.taskDone = true;
				});
			}

			if (cInfo.taskDone)
			{
				if (cInfo.pThread != nullptr)
				{
					cInfo.pThread->join();
					delete cInfo.pThread;
					cInfo.pThread = nullptr;
				}

				if (cInfo.enabled)
				{
					cInfo.enabled = cInfo.coroutine(cInfo.userData, cInfo.firstCalled) == CorCmd::Keep ? true : false;
					cInfo.firstCalled = false;
				}
				cInfo.taskStarted = false;
				cInfo.taskDone = false;
			}
		}
		++iter;
	}
}
