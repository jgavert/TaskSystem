#ifndef tasksystem_H
#define tasksystem_H
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <deque>
#include <chrono>
#include <condition_variable>
#include <functional>
#define WORKSPACE 50


struct task
{
  task(){};
  task(std::function<void()> fun, int workID)
  {
    func = fun;
    id = workID;
  };
  std::function<void()> func;
  int id;
};

class TaskSystem
{
  public:
    TaskSystem();
    TaskSystem(unsigned int threads);
    ~TaskSystem();
    void newTask(std::function<void()>, int taskID);
    bool done();
    int newWork();
    bool taskDone(int );
  private:
    void sleepAndWork(unsigned int id);
    unsigned int threadCount;
    std::deque<task*> work;
    std::vector<std::thread> workers;
    std::atomic<bool> alive;
    std::atomic<int> idleThreads;
    std::vector<int> workIDs;
    std::atomic<int> billboard[WORKSPACE];
    std::condition_variable cv;
    std::mutex mutex;
};

#endif
