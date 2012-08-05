#pragma once

#include <vector>
#include <algorithm>

#include <boost\tuple\tuple.hpp>

#include "vector.hpp"
#include "segment.hpp"

namespace sandbox {
	
class shape {
public:
	shape(std::vector<vector> const & vertices);

	std::vector<vector> const & vertices() const {
		return vertices_;
	}

	std::vector<vector> const & core() const {
		return core_;
	}

	double area() const {
		return area_;
	}

	vector centroid() const {
		return centroid_;
	}

	int unsigned support(vector const & direction) const;
	segment feature(vector const & direction) const;
		
	bool intersects(shape const & shape) const;
	boost::tuple<bool, vector, double, vector, vector> distance(shape const & shape) const;

	static std::vector<vector> transform(std::vector<vector> const & vertices, vector const & position, double const orientation);

private:
	std::vector<vector> const vertices_;
	std::vector<vector> core_;
	double area_;
	vector centroid_;
};

}