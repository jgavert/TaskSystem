#include <iostream>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>
#include <functional>
#include "tasksystem.hpp"
#define THREADS 4
#define SAMPLES 2000000000
#define WORKLOAD 20

class mathfunc
{
public:
  void pii(void* input, void* output)
  {
    long double *x = (long double*) output;
    long double *y = (long double*) input;
    *x = 0;
    for (int n = y[0]; n <= y[1]; n++)
    {
      *x += 1/pow(n,2.0);
    }
  }
};

double pii2() // singlecore bruteforce
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
  mathfunc mathlib;
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
  TaskSystem manager(THREADS);
  int workID = manager.newWork();
  //printf("First workID: %d", workID);
  for (int i=0; i<WORKLOAD;i++)
    manager.newTask([&](){mathlib.pii((void*)(input+(i*2)), (void*)(output+i));}, workID);

  while (!manager.taskDone(workID)){std::this_thread::sleep_for(std::chrono::milliseconds(10));}
  //while (!manager.taskDone(workID2)){}

  for (int i=0;i<WORKLOAD;i++)
  {
    x += output[i];
  }

  double pi = sqrt(6*(1+x));
  std::cout << "Pii approx is " << pi << std::endl;
  //std::cout << "Pii approx is " << pii2() << std::endl;
  return 0;
}
