#pragma once

#include <Eigen/Dense>
using namespace Eigen;

namespace sandbox {

class segment {
 public:
  segment(Vector2f const& a, Vector2f const& b) : a_(a), b_(b), vector_(b - a) {
  }

  Vector2f middle() const;
  Vector2f closest(Vector2f const& point) const;
  Vector2f closest(segment const& segment) const;
  float distance(Vector2f const& point) const;

  Vector2f const& a() const {
    return a_;
  }

  Vector2f const& b() const {
    return b_;
  }

  Vector2f const& getVector() const {
    return vector_;
  }

 private:
  Vector2f const a_;
  Vector2f const b_;
  Vector2f const vector_;
};
}
