#include "tasksystem.hpp"
#define WORKSPACE 50
#define VERBOSE false

std::deque<task*> work;
std::atomic<int> workCounter;
std::vector<std::thread> workers;
std::mutex mutex;
std::atomic<bool> alive;
std::atomic<int> idleThreads;
std::vector<int> workIDs;
std::atomic<int> billboard[WORKSPACE];
std::condition_variable cv;
std::mutex cv_m;

void sleepAndWork(int id)
{
  bool gotWork = false;
	task* todo;
	while(alive)
	{
		gotWork = false;
		if (workCounter == 0) {
#if VERBOSE
			printf("Thread %d: waiting for work...\n", id);
#endif
			std::unique_lock<std::mutex> lk(cv_m);
			cv.wait(lk);
#if VERBOSE
			printf("Thread %d: getting work...\n", id);
#endif
		}
		idleThreads--;
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (workCounter > 0) {
				todo = work.front();
				work.pop_front();
				workCounter--;
				gotWork = true;
			}
		}
		if (gotWork){
			todo->func();
			billboard[todo->id]--;
#if VERBOSE
			printf("Thread %d: Finished some work on %d.\n", id, todo->id);
#endif
		}
		idleThreads++;
	}
}

TaskSystem::TaskSystem()
{
  unsigned int threads = std::thread::hardware_concurrency();
	for (int i=0;i<WORKSPACE;i++)
		workIDs.push_back(WORKSPACE-i);
	alive = true;
	idleThreads = threads;
	threadCount = threads;
  for (unsigned int i=0;i<threads;i++)
    workers.push_back(std::thread(sleepAndWork, i+1));
}

TaskSystem::TaskSystem(unsigned int threads)
{
	for (int i=0;i<WORKSPACE;i++)
		workIDs.push_back(WORKSPACE-i);
	alive = true;
	idleThreads = threads;
	threadCount = threads;
  for (unsigned int i=0;i<threads;i++)
    workers.push_back(std::thread(sleepAndWork, i+1));
}

TaskSystem::~TaskSystem()
{
	alive = false;
	cv.notify_all();
  for (unsigned int i=0;i<threadCount;i++)
    workers[i].join();
}

int TaskSystem::newWork()
{
	int workID = workIDs.back();
	workIDs.pop_back();
	return workID;
}

void TaskSystem::newTask(std::function<void()> func,int taskID)
{
	std::lock_guard<std::mutex> guard(mutex);
	{
		work.push_back(new task(func, taskID));
		workCounter++;
		billboard[taskID]++;
	}
	cv.notify_all();
}

bool TaskSystem::taskDone(int taskID)
{
	if (billboard[taskID] == 0) {
		workIDs.push_back(taskID);
		return true;
	}	else return false;
}

bool TaskSystem::done()
{
	//std::cout << "stuff: " << idleThreads << ", " << threadCount << ", " << workCounter << ", " << work.size() << std::endl;
	return static_cast<unsigned int>(idleThreads) == threadCount && workCounter == 0 && work.empty();
}
