#include "scheduler.hpp"

#include <functional>

namespace sandbox {

  std::future<void> scheduler::schedule(std::function<void()> const & function) {
    std::packaged_task<void()> * const task(new std::packaged_task<void()>(function));
    auto future(task->get_future());
    tasks_.push(task);
    return future;
  }

  scheduler * scheduler::instance_ = new scheduler();

  scheduler::scheduler() : tasks_(1024) {
    for (std::size_t i(0); i < std::thread::hardware_concurrency(); ++i) {
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