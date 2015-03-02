#pragma once

#include "color.hpp"

namespace sandbox {

class material {
public:
	material(double const density, double const restitution, color<> const & color) : density_(density), restitution_(restitution), color_(color) {
	}

	double density() const {
		return density_;
	}

	double restitution() const {
		return restitution_;
	}

  color<> const & getColor() const {
    return color_;
  }

private:
	double const density_;
	double const restitution_;

  sandbox::color<> const color_;
};

}
