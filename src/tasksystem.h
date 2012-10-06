#ifndef tasksystem_H
#define	tasksystem_H
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

struct task
{
	task(){};
	task(void (*fun)(void*, void*), void* in, void* out)
	{
		func = fun;
		input = in;
		output = out;
	};
	void (*func)(void*, void*);
	void* input;
	void* output;
};

class TaskSystem
{
public:
	TaskSystem(int);
	~TaskSystem();
	void newTask(void (*func)(void*, void*), void*, void*);
	bool done();
private:
};

#endif
