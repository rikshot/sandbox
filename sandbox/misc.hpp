#pragma once

#include <vector>
#include <cmath>

#include "vector.hpp"

namespace sandbox {

static std::vector<vector> circle(double const radius, int unsigned const sections) {
	double const tau(2.0 * 3.1415926535897932384626433832795);
	std::vector<vector> vertices(sections);
	for(int unsigned i(0); i < sections; ++i) {
		double const angle(tau / sections * i);
		vertices[i] = vector(std::cos(angle), std::sin(angle)) * radius;
	}
	return vertices;
}

static std::vector<vector> rectangle(double const width, double const height) {
	double const half_width(width / 2.0);
	double const half_height(height / 2.0);
	std::vector<vector> vertices(4);
	vertices[0] = vector(half_width, -half_height);
	vertices[1] = vector(half_width, half_height);
	vertices[2] = vector(-half_width, half_height);
	vertices[3] = vector(-half_width, -half_height);
	return vertices;
}

}