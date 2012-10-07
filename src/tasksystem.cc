#include "tasksystem.h"
#define WORKSPACE 50

std::deque<task> work;
std::atomic<int> workCounter;
std::vector<std::thread> workers;
std::mutex mutex;
std::atomic<bool> alive;
std::atomic<int> idleThreads;
std::vector<int> workIDs;
std::atomic<int> billboard[WORKSPACE];

void sleepAndWork(int id)
{
	while(alive)
	{
		task* todo;
		bool fuck=false;
		while(workCounter==0&&alive){
			std::this_thread::sleep_for(std::chrono::milliseconds(40));
		}
		idleThreads--;
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (workCounter > 0) {
				todo = &work.front();
				work.pop_front();
				workCounter--;
				fuck = true;
			}
		}
		if (fuck){
			todo->func(todo->input, todo->output);
			billboard[todo->id]--;
			//printf("Thread %d: Finished some work on %d.\n", id, asd->id);
		}
		idleThreads++;
	}
}

void TaskSystem::help()
{
	while(!done())
	{
		task* todo;
		bool fuck=false;
		idleThreads--;
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (workCounter > 0) {
				todo = &work.front();
				work.pop_front();
				workCounter--;
				fuck = true;
			}
		}
		if (fuck){
			todo->func(todo->input, todo->output);
			billboard[todo->id]--;
			//printf("Main Thread: Finished some work.\n");
		}
		idleThreads++;
	}
}


TaskSystem::TaskSystem(int threads, bool helper)
{
	for (int i=0;i<WORKSPACE;i++)
		workIDs.push_back(i);
	alive = true;
	idleThreads = threads;
	threadCount = threads;
	outsideHelp = helper;
	if (!helper) {
		for (int i=0;i<threads;i++)
			workers.push_back(std::thread(sleepAndWork, i+1));
	} else {
		for (int i=0;i<threads-1;i++)
			workers.push_back(std::thread(sleepAndWork, i+1));
	}
}

TaskSystem::~TaskSystem()
{
	alive = false;
	if (outsideHelp) {
		for (int i=0;i<threadCount-1;i++)
			workers[i].join();
	} else {
		for (int i=0;i<threadCount;i++)
			workers[i].join();
	}
}

int TaskSystem::newWork()
{
	workID = workIDs.back();
	workIDs.pop_back();
	return workID;
}

void TaskSystem::newTask(void (*func)(void*, void*), void* input, void* output)
{
	std::lock_guard<std::mutex> guard(mutex);
	{
		work.push_back(task(func, input, output, workID));
		workCounter++;
		billboard[workID]++;
	}
}

bool TaskSystem::taskDone(int id)
{
	if (billboard[id] == 0) {
		workIDs.push_back(id);
		return true;
	}	else return false;
}

bool TaskSystem::done()
{
	//std::cout << "stuff: " << idleThreads << ", " << threadCount << ", " << workCounter << ", " << work.size() << std::endl;
	return idleThreads == threadCount && workCounter == 0 && work.empty();
}
