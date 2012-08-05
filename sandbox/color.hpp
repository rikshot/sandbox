#pragma once

namespace sandbox {

class color {
public:
	color() : red_(0), green_(0), blue_(0), alpha_(0) {
	}

	color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha) : red_(red), green_(green), blue_(blue), alpha_(alpha) {
	}

	unsigned int const & red() const {
		return red_;
	}

	unsigned int & red() {
		return red_;
	}

	unsigned int const & green() const {
		return green_;
	}

	unsigned int & green() {
		return green_;
	}

	unsigned int const & blue() const {
		return blue_;
	}

	unsigned int & blue() {
		return blue_;
	}

	unsigned int const & alpha() const {
		return alpha_;
	}

	unsigned int & alpha() {
		return alpha_;
	}

private:
	unsigned int red_, green_, blue_, alpha_;
};

}