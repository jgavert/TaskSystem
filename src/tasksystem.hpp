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
	TaskSystem(unsigned int);
	~TaskSystem();
	void newTask(std::function<void()>, int taskID);
	bool done();
	int newWork();
	bool taskDone(int );
private:
	unsigned int threadCount;
};

#endif
