#pragma once

#include <mutex>

#include "vector.hpp"
#include "shape.hpp"
#include "material.hpp"

namespace sandbox {

class object {
public:
	object(shape const & shape, material const & material);

	shape const & getShape() const {
		return shape_;
	}

	material const & getMaterial() const {
		return material_;
	}

	float mass() const {
		return mass_;
	}

	float moment_of_inertia() const {
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
	
	float const & orientation() const {
		return orientation_;
	}

	float & orientation() {
		return orientation_;
	}

	float const & angular_velocity() const {
		return angular_velocity_;
	}

	float & angular_velocity() {
		return angular_velocity_;
	}

	vector const & force() const {
		return force_;
	}

	vector & force() {
		return force_;
	}

	float const & torque() const {
		return torque_;
	}

	float & torque() {
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
	sandbox::shape const shape_;
	sandbox::material const material_;

	float mass_;
	float moment_of_inertia_;

	vector position_;
	vector linear_velocity_;
	float orientation_;
	float angular_velocity_;

	vector force_;
	float torque_;

	bool kinematic_;
  bool frozen_;
};

}
