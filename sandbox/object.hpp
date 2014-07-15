#pragma once

#include <mutex>

#include "vector.hpp"
#include "shape.hpp"
#include "material.hpp"

namespace sandbox {

class object {
public:
	object(shape const & shape, material const & material);

  std::mutex & mutex() {
    return mutex_;
  }

	shape const & shape() const {
		return shape_;
	}

	material const & material() const {
		return material_;
	}

	double mass() const {
		return mass_;
	}

	double moment_of_inertia() const {
		return moment_of_inertia_;
	}

	vector const & position() const {
		return position_;
	}

	vector & position() {
		return position_;
	}

	vector const & linear_velocity() const {
		return linear_velocity_;
	}

	vector & linear_velocity() {
		return linear_velocity_;
	}
	
	double const & orientation() const {
		return orientation_;
	}

	double & orientation() {
		return orientation_;
	}

	double const & angular_velocity() const {
		return angular_velocity_;
	}

	double & angular_velocity() {
		return angular_velocity_;
	}

	vector const & force() const {
		return force_;
	}

	vector & force() {
		return force_;
	}

	double const & torque() const {
		return torque_;
	}

	double & torque() {
		return torque_;
	}

	bool kinematic() const {
		return kinematic_;
	}

	void kinematic(bool const value) {
		kinematic_ = value;
	}

  bool frozen() const {
    return frozen_;
  }

  void frozen(bool const value) {
      frozen_ = value;
  }

private:
  std::mutex mutex_;

	sandbox::shape const shape_;
	sandbox::material const material_;

	double mass_;
	double moment_of_inertia_;

	vector position_;
	vector linear_velocity_;
	double orientation_;
	double angular_velocity_;

	vector force_;
	double torque_;

	bool kinematic_;
  bool frozen_;
};

}
