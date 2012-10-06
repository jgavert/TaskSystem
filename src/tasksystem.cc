#include "tasksystem.h"


std::vector<task> work;
std::atomic<int> workCounter;
std::vector<std::thread> workers;
std::mutex mutex;

void sleepAndWork()
{
	while(true)
	{
		task asd;
		bool fuck=false;
		std::lock_guard<std::mutex> guard(mutex);
		{
		if (workCounter > 0) {
			asd = work.back();
			work.pop_back();
			workCounter--;
			fuck = true;
			}
		}
		if (fuck)
			asd.func(asd.input, asd.output);
	}
}

TaskSystem::TaskSystem(int threads)
{
	for (int i=0;i<threads;i++)
		workers.push_back(std::thread(sleepAndWork));
}

TaskSystem::~TaskSystem()
{}

void TaskSystem::newTask(void (*func)(void*, void*), void* input, void* output)
{
	std::lock_guard<std::mutex> guard(mutex);
	{
		work.push_back(task(func, input, output));
	}
	workCounter++;
}

bool TaskSystem::done()
{
	return workCounter == 0;
}
