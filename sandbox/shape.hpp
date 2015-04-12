#pragma once

#include <vector>
#include <algorithm>

#include "vector.hpp"
#include "segment.hpp"
#include "rectangle.hpp"

namespace sandbox {
	
class shape {
  public:
  shape() {
  }

	shape(std::vector<vector> const & vertices) : vertices_(vertices) {}

	std::vector<vector> const & vertices() const {
		return vertices_;
	}

	shape core() const;

	float area() const;
	vector centroid() const;

  rectangle bounding_box() const;

  bool corner(vector const & vertex) const;
	int unsigned support(vector const & direction) const;
	segment feature(vector const & direction) const;
		
	bool intersects(shape const & shape) const;
	std::tuple<bool, vector, float, vector, vector> distance(shape const & shape) const;

	shape transform(vector const & position, float const orientation) const;

private:
	std::vector<vector> const vertices_;
};

}
