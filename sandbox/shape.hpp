#pragma once

#include <vector>
#include <algorithm>

#include <Eigen/Dense>
using namespace Eigen;

#include "segment.hpp"
#include "rectangle.hpp"

namespace sandbox {

class shape {
 public:
  shape() {
  }

  shape(std::vector<Vector2f> const& vertices) : vertices_(vertices) {
  }

  std::vector<Vector2f> const& vertices() const {
    return vertices_;
  }

  shape core() const;

  float area() const;
  Vector2f centroid() const;

  rectangle bounding_box() const;

  bool corner(Vector2f const& vertex) const;
  int unsigned support(Vector2f const& direction) const;
  segment feature(Vector2f const& direction) const;

  bool intersects(shape const& shape) const;
  std::tuple<bool, Vector2f, float, Vector2f, Vector2f> distance(shape const& shape) const;

  shape transform(Vector2f const& position, float const orientation) const;

 private:
  std::vector<Vector2f> const vertices_;
};
}
