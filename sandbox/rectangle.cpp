#include "rectangle.hpp"

namespace sandbox {

  bool rectangle::contains(vector const & vertex) const {
    return
      top_left_.x() <= vertex.x() &&
      top_left_.y() <= vertex.y() &&
      bottom_right_.x() >= vertex.x() &&
      bottom_right_.y() >= vertex.y();
  }

  bool rectangle::contains(rectangle const & rectangle) const {
    return
      top_left_.x() <= rectangle.top_left_.x() &&
      top_left_.y() <= rectangle.top_left_.y() &&
      bottom_right_.x() >= rectangle.bottom_right_.x() &&
      bottom_right_.y() >= rectangle.bottom_right_.y();
  }

  bool rectangle::overlaps(rectangle const & rectangle) const {
    return 
      top_left_.x() <= rectangle.bottom_right_.x() &&
      rectangle.top_left_.x() <= bottom_right_.x() &&
      top_left_.y() <= rectangle.bottom_right_.y() &&
      rectangle.top_left_.y() <= bottom_right_.y();
  }

}