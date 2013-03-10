#include "rectangle.hpp"

namespace sandbox {

bool rectangle::intersects(rectangle const & rectangle) const {
  return 
    top_left_.x() <= rectangle.bottom_right_.x() &&
    rectangle.top_left_.x() <= bottom_right_.x() &&
    top_left_.y() <= rectangle.bottom_right_.y() &&
    rectangle.top_left_.y() <= bottom_right_.y();
}

}