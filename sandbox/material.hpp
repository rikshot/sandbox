#pragma once

#include "color.hpp"

namespace sandbox {

class material {
public:
	material() : density_(), restitution_() {
	}

	material(double const density, double const restitution) : density_(density), restitution_(restitution) {
	}

	double density() const {
		return density_;
	}

	double restitution() const {
		return restitution_;
	}

private:
	double const density_;
	double const restitution_;
};

}