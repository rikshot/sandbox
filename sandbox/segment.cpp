#include "segment.hpp"

namespace sandbox {

vector segment::closest(sandbox::vector const & point) const {
	double const length_squared(vector_.length_squared());
	
	if(length_squared <= std::numeric_limits<double>::epsilon()) 
		return a_;
	
	double t((point - a_).dot(vector_) / length_squared);
	t = t > 1.0 ? 1.0 : t < 0.0 ? 0.0 : t;
	
	return vector_ * t + a_;
}

double segment::distance(sandbox::vector const & point) const {
	return (point - (a_ + vector_ * ((point - a_).dot(vector_) / vector_.length_squared()))).length();
}

}