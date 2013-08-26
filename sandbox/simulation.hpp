#pragma once

#include <vector>
#include <string>
#include <set>
#include <thread>
#include <mutex>

#include <boost\shared_ptr.hpp>

#include "object.hpp"
#include "contact.hpp"
#include "quadtree.hpp"

namespace sandbox {

class simulation {
public:
  simulation(double const width, double const height) : threads_(std::thread::hardware_concurrency()), width_(width), height_(height), time_(0.0), accumulator_(0.0) {
	}

	std::vector<boost::shared_ptr<object>> const & objects() const {
		return objects_;
	}

	std::vector<boost::shared_ptr<object>> & objects() {
		return objects_;
	}

	double time() const {
		return time_;
	}

  std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> find_collisions();
  std::vector<contact> find_contacts(std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> const & collisions) const;

	void step(double const delta_time, double const time_step);

private:
  unsigned int const threads_;

  double const width_;
  double const height_;

	std::vector<boost::shared_ptr<object>> objects_;
  std::mutex objects_mutex_;

	double time_;
	double accumulator_;

  void resolve_collisions(std::vector<contact> const & contacts);

  void resolve_contacts(std::vector<contact> const & contacts);

  boost::tuple<vector, vector, double, double> evaluate(boost::shared_ptr<object> const & initial, double const time, double const time_step, boost::tuple<vector, vector, double, double> const & derivative) const;
	void integrate(double const time_step);
};

}