#pragma once

#include <vector>
#include <cmath>
#include <thread>
#include <future>

#include "vector.hpp"
#include "scheduler.hpp"

namespace sandbox {

  template<typename Iterator, typename Function>
  void parallel_for_range(Iterator begin, Iterator end, Function function) {
    static auto const threads(std::thread::hardware_concurrency());
    auto const size(std::distance(begin, end));
    std::vector<std::function<void()>> tasks;
    auto const max_tasks(size <= threads ? size : threads);
    auto const range(size <= threads ? 1 : size / threads);
    for(unsigned int i(0), last(i + 1 == max_tasks); i < max_tasks; ++i, last = i + 1 == max_tasks) {
      tasks.emplace_back(std::function<void()>([&, i, last]() {
        auto start(begin + (range * i));
        auto const finish(last ? end : start + range);
        for(auto & j(start); j != finish; ++j) {
          function(*j);
        }
      }));
    }
    for(auto const & task : scheduler::instance()->schedule(tasks)) task.wait();
  }

  template<typename Iterator, typename Function>
  void parallel_for_range_index(Iterator begin, Iterator end, Function function) {
    static auto const threads(std::thread::hardware_concurrency());
    auto const size(std::distance(begin, end));
    std::vector<std::function<void()>> tasks;
    auto const max_tasks(size <= threads ? size : threads);
    auto const range(size <= threads ? 1 : size / threads);
    for(unsigned int i(0), last(i + 1 == max_tasks); i < max_tasks; ++i, last = i + 1 == max_tasks) {
      tasks.emplace_back(std::function<void()>([&, i, last]() {
        auto const index_start(range * i);
        auto start(begin + index_start);
        auto const finish(last ? end : start + range);
        auto index(index_start);
        for (auto & j(start); j != finish; ++j) {
          function(*j, index++);
        }
      }));
    }
    for(auto const & task : scheduler::instance()->schedule(tasks)) task.wait();
  }

}
