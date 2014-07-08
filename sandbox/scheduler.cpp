#include "scheduler.hpp"

#include <functional>

namespace sandbox {

  std::future<void> scheduler::schedule(std::function<void()> const & function) {
    auto promise(std::make_shared<std::promise<void>>());
    auto task([=] {
      function();
      promise->set_value();
    });
    auto future(promise->get_future());
    io_service_.post(task);
    return future;
  }

  scheduler * scheduler::instance_ = new scheduler();

  scheduler::scheduler() : io_service_(std::thread::hardware_concurrency()), work_(io_service_) {
    for (std::size_t i(0); i < std::thread::hardware_concurrency(); ++i) {
      std::thread(std::bind(&scheduler::runner, this)).detach();
    }
  }

  scheduler::~scheduler() {
    io_service_.stop();
  }

  void scheduler::runner() {
    io_service_.run();
  }

}