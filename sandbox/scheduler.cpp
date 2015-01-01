#include "scheduler.hpp"

#include <functional>

namespace sandbox {

  std::future<void> scheduler::schedule(std::function<void()> const & function) {
    std::packaged_task<void()> task(function);
    std::future<void> future(task.get_future());
    std::unique_lock<std::mutex> lock(task_mutex_);
    tasks_.push(std::move(task));
    lock.unlock();
    task_cv_.notify_one();
    return future;
  }

  std::vector<std::future<void>> scheduler::schedule(std::vector<std::function<void()>> const & functions) {
    std::vector<std::future<void>> futures;
    std::unique_lock<std::mutex> lock(task_mutex_);
    for (auto const & function : functions) {
      std::packaged_task<void()> task(function);
      futures.emplace_back(task.get_future());
      tasks_.push(std::move(task));
    }
    lock.unlock();
    task_cv_.notify_all();
    return futures;
  }

  scheduler * scheduler::instance_ = new scheduler();

  scheduler::scheduler() {
    for (std::size_t i(0); i < std::thread::hardware_concurrency(); ++i) {
      std::thread(std::bind(&scheduler::runner, this)).detach();
    }
  }

  void scheduler::runner() {
    for (;;) {
      std::unique_lock<std::mutex> lock(task_mutex_);
      task_cv_.wait(lock, [&] { return !tasks_.empty(); });
      std::packaged_task<void()> task(std::move(tasks_.front()));
      tasks_.pop();
      lock.unlock();
      task();
    }
  }

}
