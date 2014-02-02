#include <iostream>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>
#include <functional>
#include "tasksystem.hpp"
#include "bentsumaakaa.hpp"
#define SAMPLES 20000000
#define WORKLOAD 200

class mathfunc
{
public:
  long double pii(long double y1, long double y2)
  {
    long double x = 0;
    for (int n = y1; n <= y2; n++)
    {
      x += 1/pow(n,2.0);
    }
    return x;
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
  double pi = sqrt(6*(1+x));
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
  std::cout << "Found " << std::thread::hardware_concurrency() << " logical cores.\n";
  Bentsumaakaa timer;

  timer.start(true);
  TaskSystem manager(1);
  int workID = manager.newWork();
  for (int i=0; i<WORKLOAD;i++) {
    manager.newTask([&](){output[i] = mathlib.pii(input[i*2], input[i*2+1]);}, workID);
    //[&](){std::cout << "Task: i: " << i << ", output: " << output[i] << ", input: " << input[i*2] << ", input2: " << input[i*2] << ".\n";}();
  }

  while (!manager.taskDone(workID)){std::this_thread::sleep_for(std::chrono::milliseconds(10));}

  for (int i=0;i<WORKLOAD;i++)
  {
    x += output[i];
  }

  long double pi = sqrt(6*(1+x));
  timer.stop(true);
  /*
  long double pi = 0.0;
  TaskSystem manager;
  long workload = WORKLOAD;
  timer.bfunc([&](){
    int workID = manager.newWork();
    for (int i=0; i<workload;i++) {
      manager.newTask([&](){output[i] = mathlib.pii(input[i*2], input[i*2+1]);}, workID);
      //[&](){std::cout << "Task: i: " << i << ", output: " << output[i] << ", input: " << input[i*2] << ", input2: " << input[i*2] << ".\n";}();
    }

    while (!manager.taskDone(workID)){std::this_thread::sleep_for(std::chrono::milliseconds(10));}

    for (int i=0;i<workload;i++)
    {
      x += output[i];
    }

    pi = sqrt(6*(1+x));
    x = 0.0;
  }, 4, true);
  */
  std::cout << "multithread:  Pii approx is " << pi << std::endl;
  //std::cout << "singlethread: Pii approx is " << pii2() << std::endl;
  return 1;
}
