#pragma once

#include <cmath>
#include <limits>

namespace sandbox {

class vector {
public:
	vector(double const x = 0.0, double const y = 0.0) : x_(x), y_(y) {
	}
	
	operator bool() const {
		return x_ || y_;
	}

	bool operator !() const {
		return !x_ && !y_;
	}

	bool operator ==(vector const & rhs) const {
		return std::abs(x_ - rhs.x_) <= std::numeric_limits<double>::epsilon() && std::abs(y_ - rhs.y_) <= std::numeric_limits<double>::epsilon();
	}

	bool operator !=(vector const & rhs) const {
		return std::abs(x_ - rhs.x_) > std::numeric_limits<double>::epsilon() || std::abs(y_ - rhs.y_) > std::numeric_limits<double>::epsilon();
	}

	vector operator -() const {
		return vector(-x_, -y_);
	}

	vector operator +(vector const & rhs) const {
		return vector(x_ + rhs.x_, y_ + rhs.y_);
	}

	void operator +=(vector const & rhs) {
		x_ += rhs.x_;
		y_ += rhs.y_;
	}

	vector operator -(vector const & rhs) const {
		return vector(x_ - rhs.x_, y_ - rhs.y_);
	}

	void operator -=(vector const & rhs) {
		x_ -= rhs.x_;
		y_ -= rhs.y_;
	}

	vector operator *(double const rhs) const {
		return vector(x_ * rhs, y_ * rhs);
	}

	void operator *=(double const rhs) {
		x_ *= rhs;
		y_ *= rhs;
	}

	vector operator /(double const rhs) const {
		return vector(x_ / rhs, y_ / rhs);
	}

	void operator /=(double const rhs) {
		x_ /= rhs;
		y_ /= rhs;
	}

	double dot(vector const & rhs) const {
		return x_ * rhs.x_ + y_ * rhs.y_;
	}

	double cross(vector const & rhs) const {
		return x_ * rhs.y_ - y_ * rhs.x_;
	}

	vector cross(double const rhs) const {
		return vector(-y_ * rhs, x_ * rhs);
	}

	double length() const {
		return std::sqrt(x_ * x_ + y_ * y_);
	}

	double length_squared() const {
		return x_ * x_ + y_ * y_;
	}

	vector normalize() const {
		double const length(length());
		if(length <= std::numeric_limits<double>::epsilon()) return vector();
		return vector(x_ / length, y_ / length);
	}

	vector left() const {
		return vector(y_, -x_);
	}

	vector right() const {
		return vector(-y_, x_);
	}

	vector project(vector const & vector) const {
		return vector * (dot(vector) / vector.length_squared());
	}

	bool parallel(vector const & rhs) const {
		return std::abs(cross(rhs)) <= 10e-2;
	}

	double const & x() const {
		return x_;
	}

	double & x() {
		return x_;
	}

	double const & y() const {
		return y_;
	}

	double & y() {
		return y_;
	}

	static vector triple_product(vector const & a, vector const & b, vector const & c) {
		return b * c.dot(a) - a * c.dot(b);
	}

private:
	double x_;
	double y_;
};

}