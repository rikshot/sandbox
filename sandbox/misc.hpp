#pragma once

#include <vector>
#include <cmath>
#include <thread>
#include <future>

#include "scheduler.hpp"

namespace sandbox {

static float cross(Vector2f const& a, Vector2f const& b) {
  return a.x() * b.y() - a.y() * b.x();
}

static Vector2f cross(Vector2f const& a, float const b) {
  return Vector2f(-a.y() * b, a.x() * b);
}

static Vector2f triple_product_left(Vector2f const& a, Vector2f const& b, Vector2f const& c) {
  return b * c.dot(a) - a * c.dot(b);
}

static Vector2f triple_product_right(Vector2f const& a, Vector2f const& b, Vector2f const& c) {
  return b * a.dot(c) - c * a.dot(b);
}

static bool parallel(Vector2f const& a, Vector2f const& b) {
  return std::abs(cross(a, b)) <= 0.1;
}

template <typename Iterator, typename Function>
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
      for(auto& j(start); j != finish; ++j) {
        function(*j);
      }
    }));
  }
  for(auto const& task : scheduler::instance()->schedule(tasks))
    task.wait();
}

template <typename Iterator, typename Function>
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
      for(auto& j(start); j != finish; ++j) {
        function(*j, index++);
      }
    }));
  }
  for(auto const& task : scheduler::instance()->schedule(tasks))
    task.wait();
}
}
