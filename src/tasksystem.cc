#include "tasksystem.h"
#include <iostream>


std::vector<task> work;
std::atomic<int> workCounter;
std::vector<std::thread> workers;
std::mutex mutex;
std::atomic<bool> alive;
std::atomic<int> idleThreads;

void sleepAndWork()
{
	while(alive)
	{
		task asd;
		bool fuck=false;
		idleThreads--;
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (workCounter > 0) {
				asd = work.back();
				work.pop_back();
				workCounter--;
				fuck = true;
			}
		}
		if (fuck){
			asd.func(asd.input, asd.output);
		}
		idleThreads++;
	}
}
void TaskSystem::help()
{
	while(work.size()!=0)
	{
		task asd;
		bool fuck=false;
		idleThreads--;
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (workCounter > 0) {
				asd = work.back();
				work.pop_back();
				workCounter--;
				fuck = true;
			}
		}
		if (fuck){
			asd.func(asd.input, asd.output);
		}
		idleThreads++;
	}
}


TaskSystem::TaskSystem(int threads)
{
	alive = true;
	idleThreads = threads;
	threadCount = threads;
	for (int i=0;i<threads-1;i++)
		workers.push_back(std::thread(sleepAndWork));
}

TaskSystem::~TaskSystem()
{
	alive = false;
	for (int i=0;i<threadCount-1;i++)
		workers[i].join();
}

void TaskSystem::newTask(void (*func)(void*, void*), void* input, void* output)
{
	std::lock_guard<std::mutex> guard(mutex);
	{
		work.push_back(task(func, input, output));
		workCounter++;
	}
}

bool TaskSystem::done()
{
	//std::cout << "stuff: " << idleThreads << ", " << threadCount << ", " << workCounter << ", " << work.size() << std::endl;
	return idleThreads == threadCount && workCounter == 0 && work.empty();
}
