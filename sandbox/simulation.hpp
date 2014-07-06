#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <thread>
#include <mutex>

#include "object.hpp"
#include "contact.hpp"
#include "quadtree.hpp"

namespace sandbox {

class simulation {
public:
  simulation(double const width, double const height) : width_(width), height_(height), time_(0.0), accumulator_(0.0) {
	}

	std::vector<std::shared_ptr<object>> const & objects() const {
		return objects_;
	}

	std::vector<std::shared_ptr<object>> & objects() {
		return objects_;
	}

	double time() const {
		return time_;
	}

  std::set<std::pair<std::shared_ptr<object>, std::shared_ptr<object>>> find_collisions();
  std::vector<contact> find_contacts(std::set<std::pair<std::shared_ptr<object>, std::shared_ptr<object>>> const & collisions) const;

	void step(double const delta_time, double const time_step);

private:
  double const width_;
  double const height_;

	std::vector<std::shared_ptr<object>> objects_;
  std::mutex objects_mutex_;

	double time_;
	double accumulator_;

  void resolve_collisions(std::vector<contact> const & contacts);

  void resolve_contacts(std::vector<contact> const & contacts);

  std::tuple<vector, vector, double, double> evaluate(std::shared_ptr<object> const & initial, double const time, double const time_step, std::tuple<vector, vector, double, double> const & derivative) const;
	void integrate(double const time_step);
};

}