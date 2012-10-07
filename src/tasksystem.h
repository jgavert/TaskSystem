#ifndef tasksystem_H
#define	tasksystem_H
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <deque>
#include <chrono>

struct task
{
	task(){};
	task(void (*fun)(void*, void*), void* in, void* out, int workID)
	{
		func = fun;
		input = in;
		output = out;
		id = workID;
	};
	void (*func)(void*, void*);
	void* input;
	void* output;
	int id;
};

class TaskSystem
{
public:
	TaskSystem(int, bool);
	~TaskSystem();
	void newTask(void (*func)(void*, void*), void*, void*);
	bool done();
	void help();
	int newWork();
	bool taskDone(int );
private:
	int threadCount;
	bool outsideHelp;
	int workID;

};

#endif
