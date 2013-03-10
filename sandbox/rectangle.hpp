#pragma once

#include <vector>

#include "vector.hpp"

namespace sandbox {
	
class rectangle {
public:
	rectangle(vector const & top_left, vector const & bottom_right) : top_left_(top_left), bottom_right_(bottom_right) {
    vertices_.push_back(top_left);
    vertices_.push_back(vector(bottom_right.x(), top_left.y()));
    vertices_.push_back(bottom_right);
    vertices_.push_back(vector(top_left.x(), bottom_right.y()));
  }

  std::vector<vector> const & vertices() const {
    return vertices_;
  }
		
	bool intersects(rectangle const & rectangle) const;

private:
  std::vector<vector> vertices_;
  vector const top_left_;
  vector const bottom_right_;
};

}