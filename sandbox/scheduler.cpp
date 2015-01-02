#include "scheduler.hpp"

#include <functional>

namespace sandbox {

  std::future<void> scheduler::schedule(std::function<void()> const & function) {
    auto promise(std::make_shared<std::promise<void>>());
    io_service_.post([=] {
      function();
      promise->set_value();
    });
    return promise->get_future();
  }

  std::vector<std::future<void>> scheduler::schedule(std::vector<std::function<void()>> const & functions) {
    std::vector<std::future<void>> futures;
    for (auto const & function : functions) {
      auto promise(std::make_shared<std::promise<void>>());
      io_service_.post([=] {
        function();
        promise->set_value();
      });
      futures.emplace_back(promise->get_future());
    }
    return futures;
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
