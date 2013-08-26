#pragma once

#include <vector>
#include <cmath>

#include "vector.hpp"

namespace sandbox {

static std::vector<vector> circle(double const radius, int unsigned const sections) {
	static double const tau(2.0 * 3.1415926535897932384626433832795);
	std::vector<vector> vertices(sections);
	for(int unsigned i(0); i < sections; ++i) {
		double const angle(tau / sections * i);
		vertices[i] = vector(std::cos(angle), std::sin(angle)) * radius;
	}
	return vertices;
}

}