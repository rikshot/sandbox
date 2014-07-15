#pragma once

#include <vector>

#include "vector.hpp"

namespace sandbox {

  class rectangle {
    public:
      rectangle() {
      }

      rectangle(double const width, double const height) : top_left_(-(width / 2), -(height / 2)), bottom_right_(width / 2, height / 2) {
      }

      rectangle(vector const & top_left, vector const & bottom_right) : top_left_(top_left), bottom_right_(bottom_right) {
      }

      vector const & top_left() const {
        return top_left_;
      }

      vector const & bottom_right() const {
        return bottom_right_;
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

    private:
      vector const top_left_;
      vector const bottom_right_;
  };

}
