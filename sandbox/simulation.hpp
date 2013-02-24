#pragma once

#include <map>
#include <vector>
#include <string>

#include <boost\shared_ptr.hpp>

#include "object.hpp"
#include "contact.hpp"

namespace sandbox {

class simulation {
public:
	simulation() : time_(0.0), accumulator_(0.0) {
	}

	std::vector<boost::shared_ptr<object>> const & objects() const {
		return objects_;
	}

	std::vector<boost::shared_ptr<object>> & objects() {
		return objects_;
	}

	std::vector<contact> const & contacts() const {
		return contacts_;
	}

	double time() const {
		return time_;
	}

	void step(double const delta_time, double const time_step);

private:
	std::vector<boost::shared_ptr<object>> objects_;
	std::vector<contact> contacts_;

	double time_;
	double accumulator_;

	void integrate(double const time_step);
};

}