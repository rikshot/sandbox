#pragma once

#include <vector>
#include <cmath>
#include <thread>
#include <future>

#include "vector.hpp"
#include "scheduler.hpp"

namespace sandbox {

  static std::vector<vector> circle(double const radius, int unsigned const sections) {
    static double const tau(2.0 * 3.1415926535897932384626433832795);
    std::vector<vector> vertices(sections);
    for (int unsigned i(0); i < sections; ++i) {
      double const angle(tau / sections * i);
      vertices[i] = vector(std::cos(angle), std::sin(angle)) * radius;
    }
    return vertices;
  }

  template<typename Iterator, typename Function>
  void parallel_for(Iterator begin, Iterator end, Function function) {
    std::vector<std::future<void>> tasks;
    for (Iterator i(begin); i != end; ++i) {
      tasks.emplace_back(scheduler::instance()->schedule(std::bind(function, *i)));
    }
    for (auto const & task : tasks) task.wait();
  }

}