#include "segment.hpp"

namespace sandbox {

Vector2f segment::middle() const {
  return Vector2f((a_.x() + b_.x()) / 2, (a_.y() + b_.y()) / 2);
}

Vector2f segment::closest(Vector2f const& point) const {
  float const length_squared(vector_.squaredNorm());

  if(length_squared <= std::numeric_limits<float>::epsilon())
    return a_;

  float t((point - a_).dot(vector_) / length_squared);
  t = t > 1.0 ? 1.0 : t < 0.0 ? 0.0 : t;

  return vector_ * t + a_;
}

Vector2f segment::closest(segment const& segment) const {
  if(distance(segment.a_) < distance(segment.b_)) {
    return a_;
  }
  return b_;
}

float segment::distance(Vector2f const& point) const {
  return (point - (a_ + vector_ * ((point - a_).dot(vector_) / vector_.squaredNorm()))).norm();
}
}
