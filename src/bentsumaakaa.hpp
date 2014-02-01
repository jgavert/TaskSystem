#ifndef BENTSUMAAKAA_HPP
#define BENTSUMAAKAA_HPP
#include <iostream>
#include <chrono>
#include <functional>

// Because normal names like 'Benchmarker' or 'Benchmarksystem' are bit boring.
class Bentsumaakaa
{
public:
  Bentsumaakaa();
  ~Bentsumaakaa();
  void start(const bool verbose);
  long stop(const bool verbose);
  long bfunc(const std::function<void()> block, const int times, const bool verbose);

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> startpoint;
};

#endif

