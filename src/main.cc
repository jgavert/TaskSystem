#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include "tasksystem.h"


void foo(void* data, void* output)
{
	int* x = (int*) data;
	int* y = (int*) output;
	*y = (*x)*(*x);
}

int main(void)
{
	int *data = new int[10]{2, 3, 4, 5, 6, 7 ,8 ,9 ,8 ,7};
	int *output = new int[10]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	foo( (void*)(data+1), (void*)(output+1));
	std::cout << "Test without TaskSystem: " << output[1] << std::endl;
	TaskSystem manager(4);
	for (int i=0; i<10;i++)
		manager.newTask(foo, (void*)(data+i), (void*)(output+i));
	while (!manager.done()){}
	std::cout << output[0];
	for (int i=1;i<10;i++)
		std::cout << ", " << output[i];
	std::cout << std::endl;
	return 0;
}