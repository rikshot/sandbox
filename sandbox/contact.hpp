#pragma once

#include <memory>

#include <Eigen/Dense>
using namespace Eigen;

#include "object.hpp"
#include "misc.hpp"

namespace sandbox {

class contact {
 public:
  contact(std::shared_ptr<object> const& a,
          std::shared_ptr<object> const& b,
          Vector2f const& ap,
          Vector2f const& bp,
          Vector2f const& normal)
      : a_(a), b_(b), ap_(ap), bp_(bp), normal_(normal) {
  }

  std::shared_ptr<object> a() const {
    return a_;
  }

  std::shared_ptr<object> b() const {
    return b_;
  }

  Vector2f const& ap() const {
    return ap_;
  }

  Vector2f const& bp() const {
    return bp_;
  }

  Vector2f const& normal() const {
    return normal_;
  }

  float relative_velocity() const {
    Vector2f const ra(a_->position() - ap_);
    Vector2f const rb(b_->position() - bp_);
    Vector2f const vab(b_->linear_velocity() - sandbox::cross(rb, b_->angular_velocity()) - a_->linear_velocity() +
                       sandbox::cross(ra, a_->angular_velocity()));
    return vab.dot(normal_);
  }

 private:
  std::shared_ptr<object> a_;
  std::shared_ptr<object> b_;
  Vector2f ap_;
  Vector2f bp_;
  Vector2f normal_;
};
}
