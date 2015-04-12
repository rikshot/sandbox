#pragma once

#include "color.hpp"

namespace sandbox {

class material {
public:
	material(float const density, float const restitution, color<> const & color) : density_(density), restitution_(restitution), color_(color) {
	}

	float density() const {
		return density_;
	}

	float restitution() const {
		return restitution_;
	}

  color<> const & getColor() const {
    return color_;
  }

private:
	float const density_;
	float const restitution_;

  sandbox::color<> const color_;
};

}
