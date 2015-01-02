#pragma once

#include <future>

#include <boost/asio.hpp>

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

    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;

    scheduler();
    ~scheduler();

    void runner();

  };

}
