#pragma once

#include <vector>
#include <cmath>
#include <thread>
#include <future>

#include "vector.hpp"

namespace sandbox {

static std::vector<vector> circle(double const radius, int unsigned const sections) {
	static double const tau(2.0 * 3.1415926535897932384626433832795);
	std::vector<vector> vertices(sections);
	for(int unsigned i(0); i < sections; ++i) {
		double const angle(tau / sections * i);
		vertices[i] = vector(std::cos(angle), std::sin(angle)) * radius;
	}
	return vertices;
}

template<typename Iterator, typename Function>
void parallel_for(Iterator begin, Iterator end, Function function) {
  static auto const threads(std::thread::hardware_concurrency());
  auto const size(std::distance(begin, end));
  std::vector<std::future<void>> tasks;
  auto const max_tasks(size <= threads ? size : threads);
  auto const range(size <= threads ? 1 : size / threads);
  for(unsigned int i(0), last(i + 1 == max_tasks); i < max_tasks; ++i, last = i + 1 == max_tasks) {
    tasks.emplace_back(std::async([&, i, last]() {
      auto start(begin + (range * i));
      auto const finish(last ? end : start + range);
      for(auto & j(start); j != finish; ++j) {
        function(*j);
      }
    }));
  }
  for(auto const & task : tasks) task.wait();
}

}
