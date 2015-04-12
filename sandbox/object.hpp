#pragma once

#include <mutex>

#include <Eigen/Dense>
using namespace Eigen;

#include "shape.hpp"
#include "material.hpp"

namespace sandbox {

class object {
 public:
  object(shape const& shape, material const& material);

  shape const& getShape() const {
    return shape_;
  }

  material const& getMaterial() const {
    return material_;
  }

  float mass() const {
    return mass_;
  }

  float moment_of_inertia() const {
    return moment_of_inertia_;
  }

  Vector2f const& position() const {
    return position_;
  }

  Vector2f& position() {
    return position_;
  }

  Vector2f const& linear_velocity() const {
    return linear_velocity_;
  }

  Vector2f& linear_velocity() {
    return linear_velocity_;
  }

  float const& orientation() const {
    return orientation_;
  }

  float& orientation() {
    return orientation_;
  }

  float const& angular_velocity() const {
    return angular_velocity_;
  }

  float& angular_velocity() {
    return angular_velocity_;
  }

  Vector2f const& force() const {
    return force_;
  }

  Vector2f& force() {
    return force_;
  }

  float const& torque() const {
    return torque_;
  }

  float& torque() {
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

  Vector2f position_;
  Vector2f linear_velocity_;
  float orientation_;
  float angular_velocity_;

  Vector2f force_;
  float torque_;

  bool kinematic_;
  bool frozen_;
};
}
