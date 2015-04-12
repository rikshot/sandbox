#pragma once

#include <cmath>
#include <limits>

namespace sandbox {

class vector {
 public:
  vector(float const x = 0.0f, float const y = 0.0f) : x_(x), y_(y) {
  }

  operator bool() const {
    return x_ || y_;
  }

  bool operator!() const {
    return !x_ && !y_;
  }

  bool operator==(vector const &rhs) const {
    return std::abs(x_ - rhs.x_) <= 0.1f && std::abs(y_ - rhs.y_) <= 0.1f;
  }

  bool operator!=(vector const &rhs) const {
    return std::abs(x_ - rhs.x_) > 0.1f || std::abs(y_ - rhs.y_) > 0.1f;
  }

  vector operator-() const {
    return vector(-x_, -y_);
  }

  vector operator+(vector const &rhs) const {
    return vector(x_ + rhs.x_, y_ + rhs.y_);
  }

  void operator+=(vector const &rhs) {
    x_ += rhs.x_;
    y_ += rhs.y_;
  }

  vector operator-(vector const &rhs) const {
    return vector(x_ - rhs.x_, y_ - rhs.y_);
  }

  void operator-=(vector const &rhs) {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
  }

  vector operator*(float const rhs) const {
    return vector(x_ * rhs, y_ * rhs);
  }

  void operator*=(float const rhs) {
    x_ *= rhs;
    y_ *= rhs;
  }

  vector operator/(float const rhs) const {
    return vector(x_ / rhs, y_ / rhs);
  }

  void operator/=(float const rhs) {
    x_ /= rhs;
    y_ /= rhs;
  }

  float dot(vector const &rhs) const {
    return x_ * rhs.x_ + y_ * rhs.y_;
  }

  float cross(vector const &rhs) const {
    return x_ * rhs.y_ - y_ * rhs.x_;
  }

  vector cross(float const rhs) const {
    return vector(-y_ * rhs, x_ * rhs);
  }

  float length() const {
    return std::sqrt(x_ * x_ + y_ * y_);
  }

  float length_squared() const {
    return x_ * x_ + y_ * y_;
  }

  vector normalize() const {
    float const length(this->length());
    if(length <= std::numeric_limits<float>::epsilon())
      return vector();
    return vector(x_ / length, y_ / length);
  }

  vector left() const {
    return vector(y_, -x_);
  }

  vector right() const {
    return vector(-y_, x_);
  }

  vector project(vector const &vector) const {
    return vector * (dot(vector) / vector.length_squared());
  }

  bool parallel(vector const &rhs) const {
    return std::abs(cross(rhs)) <= 0.1f;
  }

  float const &x() const {
    return x_;
  }

  float &x() {
    return x_;
  }

  float const &y() const {
    return y_;
  }

  float &y() {
    return y_;
  }

  static vector triple_product_left(vector const &a, vector const &b, vector const &c) {
    return b * c.dot(a) - a * c.dot(b);
  }

  static vector triple_product_right(vector const &a, vector const &b, vector const &c) {
    return b * a.dot(c) - c * a.dot(b);
  }

 private:
  float x_;
  float y_;
};
}
