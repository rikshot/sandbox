#pragma once

#include "vector.hpp"

namespace sandbox {

class segment {
public:
	segment(vector const & a, vector const & b) : a_(a), b_(b), vector_(b - a) {
	}

  vector middle() const;
	vector closest(vector const & point) const;
  vector closest(segment const & segment) const;
	double distance(vector const & point) const;

	vector const & a() const {
		return a_;
	}

	vector const & b() const {
		return b_;
	}

	vector const & vector() const {
		return vector_;
	}

private:
	sandbox::vector const a_;
	sandbox::vector const b_;
	sandbox::vector const vector_;
};

}