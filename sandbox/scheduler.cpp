#include "scheduler.hpp"

#include <functional>

namespace sandbox {

  std::future<void> scheduler::schedule(std::function<void()> const & function) {
    std::packaged_task<void()> * const task(new std::packaged_task<void()>(function));
    auto future(task->get_future());
    tasks_.push(task);
    return future;
  }

  std::vector<std::future<void>> scheduler::schedule(std::vector<std::function<void()>> const & functions) {
    std::vector<std::future<void>> futures;
    tasks_.reserve(functions.size());
    for (auto const & function : functions) {
      std::packaged_task<void()> * const task(new std::packaged_task<void()>(function));
      futures.emplace_back(task->get_future());
      tasks_.push(task);
    }
    return futures;
  }

  scheduler * scheduler::instance_ = new scheduler();

  scheduler::scheduler() : tasks_(1 << 16) {
    for (std::size_t i(0); i < std::thread::hardware_concurrency() - 1; ++i) {
      std::thread(std::bind(&scheduler::runner, this)).detach();
    }
  }

  void scheduler::runner() {
    for (;;) {
      tasks_.consume_one([](std::packaged_task<void()> * const task) {
        task->operator()();
        delete task;
      });
    }
  }

}
