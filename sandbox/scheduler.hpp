#pragma once

#include <future>

#include <boost/lockfree/queue.hpp>

namespace sandbox {

  class scheduler {
  
  public:
    static scheduler * const instance() {
      return instance_;
    }

    std::future<void> schedule(std::function<void()> const & function);

  private:
    static scheduler * instance_;

    boost::lockfree::queue<std::packaged_task<void()> *> tasks_;

    scheduler();
    ~scheduler();

    void runner();

  };

}