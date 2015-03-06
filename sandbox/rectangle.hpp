#pragma once

#include <vector>

#include "vector.hpp"

namespace sandbox {

  class rectangle {
    public:
      rectangle() : width_(0.0), height_(0.0) {
      }

      rectangle(double const width, double const height) : top_left_(-(width / 2), -(height / 2)), bottom_right_(width / 2, height / 2), width_(width), height_(height) {
      }

      rectangle(vector const & top_left, vector const & bottom_right) : top_left_(top_left), bottom_right_(bottom_right), width_(bottom_right.x() - top_left.x()), height_(bottom_right.y() - top_left.y()) {
      }

      vector const & top_left() const {
        return top_left_;
      }

      vector const & bottom_right() const {
        return bottom_right_;
      }

      double width() const {
        return width_;
      }

      double height() const {
        return height_;
      }

      std::vector<vector> vertices() const {
        std::vector<vector> vertices;
        vertices.push_back(top_left_);
        vertices.push_back(vector(bottom_right_.x(), top_left_.y()));
        vertices.push_back(bottom_right_);
        vertices.push_back(vector(top_left_.x(), bottom_right_.y()));
        return vertices;
      }

      bool contains(vector const & vertex) const;
      bool contains(rectangle const & rectangle) const;
      bool overlaps(rectangle const & rectangle) const;

      static rectangle create_union(rectangle const & a, rectangle const & b);

    private:
      vector top_left_;
      vector bottom_right_;

      double width_;
      double height_;
  };

}
