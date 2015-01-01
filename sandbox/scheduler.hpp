#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <future>
#include <condition_variable>

#include "workarounds.hpp"

namespace sandbox {

  class scheduler {
  
  public:
    static scheduler * const instance() {
      return instance_;
    }

    std::future<void> schedule(std::function<void()> const & function);
    std::vector<std::future<void>> schedule(std::vector<std::function<void()>> const & functions);

  private:
    static scheduler * instance_;

    std::queue<std::packaged_task<void()>> tasks_;
    std::mutex task_mutex_;
    std::condition_variable task_cv_;

    scheduler();

    void runner();

  };

}
