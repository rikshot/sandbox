#pragma once

namespace sandbox {

template<typename T = double>
class color {
public:
	color(T const red, T const green, T const blue, T const alpha) : red_(red), green_(green), blue_(blue), alpha_(alpha) {}

	T const & red() const {
		return red_;
	}

	T const & green() const {
		return green_;
	}

	T const & blue() const {
		return blue_;
	}

	T const & alpha() const {
		return alpha_;
	}

private:
	T const red_, green_, blue_, alpha_;
};

}