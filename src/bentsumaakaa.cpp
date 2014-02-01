#include "bentsumaakaa.hpp"

Bentsumaakaa::Bentsumaakaa()
{
}
Bentsumaakaa::~Bentsumaakaa()
{
}
void Bentsumaakaa::start(const bool verbose)
{
  if (verbose)
    std::cout << "Bentsumaakaa: Start()\n";
  startpoint = std::chrono::high_resolution_clock::now();
}
long Bentsumaakaa::stop(const bool verbose)
{
  auto endpoint = std::chrono::high_resolution_clock::now();
  long time = std::chrono::duration_cast<std::chrono::milliseconds>(endpoint - startpoint).count();
  if (verbose)
    std::cout << "Bentsumaakaa: stop(), time was " << time << "ms.\n";
  return time;
}
long Bentsumaakaa::bfunc(const std::function<void()> block, const int times, const bool verbose)
{
  if (verbose)
    std::cout << "Bentsumaakaa: Benchmarking function for " << times << " times.\n";
  auto point1 = std::chrono::high_resolution_clock::now();
  for (int i=0;i<times;i++) {
    block();
  }
  auto point2 = std::chrono::high_resolution_clock::now();
  long time = std::chrono::duration_cast<std::chrono::milliseconds>(point2 - point1).count();
  if (verbose)
    std::cout << "Bentsumaakaa: Benchmarking finished, average time was " << time/static_cast<long>(times) << "ms.\n";
  return time/static_cast<long>(times);
}
