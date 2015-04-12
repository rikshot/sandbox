#pragma once

#include <vector>

#include <Eigen/Dense>
using namespace Eigen;

namespace sandbox {

class rectangle {
 public:
  rectangle() : width_(0.0), height_(0.0) {
  }

  rectangle(float const width, float const height)
      : top_left_(-(width / 2), -(height / 2)), bottom_right_(width / 2, height / 2), width_(width), height_(height) {
  }

  rectangle(Vector2f const& top_left, Vector2f const& bottom_right)
      : top_left_(top_left)
      , bottom_right_(bottom_right)
      , width_(bottom_right.x() - top_left.x())
      , height_(bottom_right.y() - top_left.y()) {
  }

  Vector2f const& top_left() const {
    return top_left_;
  }

  Vector2f const& bottom_right() const {
    return bottom_right_;
  }

  float width() const {
    return width_;
  }

  float height() const {
    return height_;
  }

  std::vector<Vector2f> vertices() const {
    std::vector<Vector2f> vertices;
    vertices.push_back(top_left_);
    vertices.push_back(Vector2f(bottom_right_.x(), top_left_.y()));
    vertices.push_back(bottom_right_);
    vertices.push_back(Vector2f(top_left_.x(), bottom_right_.y()));
    return vertices;
  }

  bool contains(Vector2f const& vertex) const;
  bool contains(rectangle const& rectangle) const;
  bool overlaps(rectangle const& rectangle) const;

  static rectangle create_union(rectangle const& a, rectangle const& b);

 private:
  Vector2f top_left_;
  Vector2f bottom_right_;

  float width_;
  float height_;
};
}
