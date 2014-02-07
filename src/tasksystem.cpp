#include "tasksystem.hpp"
#define VERBOSE 0
#define WORKSPACE 50


void TaskSystem::sleepAndWork(unsigned int id)
{
  bool gotWork = false;
  task* todo;
  while(alive)
  {
    gotWork = false;
    {
      std::unique_lock<std::mutex> lk(mutex);
      if (work.size() == 0) {
#if VERBOSE
        printf("Thread %d: waiting for work...\n", id);
#endif
        idleThreads++;
        cv.wait(lk);
        idleThreads--;
#if VERBOSE
        printf("Thread %d: getting work...\n", id);
#endif
      }
    }
    {
      std::lock_guard<std::mutex> guard(mutex);
      if (work.size() > 0) {
        //printf("Thread %d: work left %d\n", id, work.size());
        todo = work.front();
        work.pop_front();
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
  }
}

TaskSystem::TaskSystem():
  threadCount(std::thread::hardware_concurrency()),
  alive(true),
  idleThreads(std::thread::hardware_concurrency())
{
  for (int i=0;i<WORKSPACE;i++)
    billboard[i] = 0;
  for (int i=0;i<WORKSPACE;i++)
    workIDs.push_back(WORKSPACE-i);
  for (unsigned int i=0;i<threadCount;i++)
    workers.push_back(std::thread(&TaskSystem::sleepAndWork, this, threadCount-i));
}

TaskSystem::TaskSystem(unsigned int threads):
  threadCount(threads),
  alive(true),
  idleThreads(threads)
{
  for (int i=0;i<WORKSPACE;i++)
    billboard[i] = 0;
  for (int i=0;i<WORKSPACE;i++)
    workIDs.push_back(WORKSPACE-i);
  for (unsigned int i=0;i<threadCount;i++)
    workers.push_back(std::thread(&TaskSystem::sleepAndWork, this, threadCount-i));
}

TaskSystem::~TaskSystem()
{
  alive = false;
  cv.notify_all();
  for (unsigned int i=0;i<threadCount;i++)
    workers[i].join();
}

// Returns a unique ID for which to add work for.
int TaskSystem::newWork()
{
  int workID = workIDs.back();
  workIDs.pop_back();
  return workID;
}

// Adds a new task for the workers to chew on
void TaskSystem::newTask(std::function<void()> func,int taskID)
{
  {
    std::lock_guard<std::mutex> guard(mutex);
    work.push_back(new task(func, taskID));
    billboard[taskID]++;
  }
  cv.notify_one();
}

// This method is probably the one you will use to check if work is done.
bool TaskSystem::taskDone(int taskID)
{
  if (billboard[taskID] == 0) {
    workIDs.push_back(taskID);
    return true;
  }
    else return false;
}

// General method for asking if we are done
bool TaskSystem::done()
{
  std::lock_guard<std::mutex> guard(mutex);
  //std::cout << "stuff: " << idleThreads << ", " << threadCount << ", " << workCounter << ", " << work.size() << std::endl;
  return static_cast<unsigned int>(idleThreads) == threadCount && work.empty();
}
