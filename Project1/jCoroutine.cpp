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
	mCoroutines[name].enabled = false;
}

void jCoroutine::RunCoroutines()
{
	for (auto iter = mCoroutines.begin(); iter != mCoroutines.end();)
	{
		CoroutineInfo& cInfo = iter->second;
		if (cInfo.enabled == false)
		{
			mCoroutines.erase(iter++);
			continue;
		}

		if (cInfo.mode == CoroutineMode::Normal) //�ڷ�ƾ ȣ�� �� false��ȯ�� ����
		{
			cInfo.enabled = cInfo.coroutine() == CoroutineReturn::Keep ? true : false;
		}
		else if (cInfo.mode == CoroutineMode::Timer) //�����ð� ���� �� �ڷ�ƾ ȣ��
		{
			cInfo.time_ms -= jTime::Delta() * 1000.0;
			if (cInfo.time_ms <= 0)
			{
				cInfo.enabled = cInfo.coroutine() == CoroutineReturn::Keep ? true : false;
				cInfo.time_ms = cInfo.time_back_ms;
			}
		}
		else if (cInfo.mode == CoroutineMode::Task) //������ �Ϸ� �� �ڷ�ƾ ȣ��
		{
			if (cInfo.taskStarted == false)
			{
				cInfo.taskStarted = true;
				cInfo.taskDone = false;
				thread t1([&cInfo]() {
					cInfo.task();
					cInfo.taskDone = true;
				});
			}

			if (cInfo.taskDone)
			{
				cInfo.enabled = cInfo.coroutine() == CoroutineReturn::Keep ? true : false;
				cInfo.taskStarted = false;
				cInfo.taskDone = false;
			}
		}
		++iter;
	}
}
