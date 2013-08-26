#pragma once

#include <vector>
#include <algorithm>

#include <boost\tuple\tuple.hpp>

#include "vector.hpp"
#include "segment.hpp"
#include "rectangle.hpp"

namespace sandbox {
	
class shape {
public:
	shape(std::vector<vector> const & vertices) : vertices_(vertices) {}

	std::vector<vector> const & vertices() const {
		return vertices_;
	}

	shape core() const;

	double area() const;
	vector centroid() const;

  rectangle bounding_box() const;

  bool corner(vector const & vertex) const;
	int unsigned support(vector const & direction) const;
	segment feature(vector const & direction) const;
		
	bool intersects(shape const & shape) const;
	boost::tuple<bool, vector, double, vector, vector> distance(shape const & shape) const;

	shape transform(vector const & position, double const orientation) const;

private:
	std::vector<vector> const vertices_;
};

}