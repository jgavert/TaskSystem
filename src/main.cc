#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <math.h>
#include "tasksystem.h"
#define THREADS 4
//#define SAMPLES 1000000000
#define SAMPLES 1000000000
#define WORKLOAD 10

void pii(void* input, void* output)
{
	long double *x = (long double*) output;
	long double *y = (long double*) input;
	//printf("start: %Lf, end %Lf\n", y[0], y[1]);
	*x = 0;
	for (int n = y[0]; n <= y[1]; n++)
	{
	  *x += 1/pow(n,2.0);
	  //printf("%Lf\n", *x);
	}
	//printf("%Lf\n", *x);
	//printf("Finished something!\n");
}

double pii2()
{
long double x=0;
//printf("%Lf\n", x);
for (int n = 2; n <= SAMPLES; n++)
{
  x += 1/pow(n,2.0);
}
//printf("%Lf\n", x);
double pi =sqrt(6*(1+x));
return pi;
}


int main(void)
{
	long double x = 0;
	long double *output = new long double[WORKLOAD];
	long double *input = new long double[WORKLOAD*2];

	long double splitter = SAMPLES/WORKLOAD;
	long double z = 1;
	for (int i=0;i<WORKLOAD*2;i+=2)
	{
		input[i] = z+1;
		z += splitter;
		input[i+1] = z;
	}
	TaskSystem manager(THREADS, false);
	int workID = manager.newWork();
	printf("First workID: %d", workID);
	for (int i=0; i<WORKLOAD;i++)
		manager.newTask(pii, (void*)(input+(i*2)), (void*)(output+i));
	int workID2 = manager.newWork();
	printf("Second workID: %d", workID2);
	for (int i=0; i<WORKLOAD;i++)
		manager.newTask(pii, (void*)(input+(i*2)), (void*)(output+i));
	//manager.help();
	while (!manager.taskDone(workID)){}
	while (!manager.taskDone(workID2)){}

	for (int i=0;i<WORKLOAD;i++)
	{
		x += output[i];
	}

	double pi = sqrt(6*(1+x));
	std::cout << "Pii approx is " << pi << std::endl;
	//std::cout << "Pii approx is " << pii2() << std::endl;
	return 0;
}
