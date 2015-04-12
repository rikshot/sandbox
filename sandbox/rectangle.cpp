#include "rectangle.hpp"

#include <algorithm>

namespace sandbox {

  bool rectangle::contains(Vector2f const & vertex) const {
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

  rectangle rectangle::create_union(rectangle const & a, rectangle const & b) {
    auto const x1(std::min(a.top_left_.x(), b.top_left_.x()));
    auto const x2(std::max(a.bottom_right_.x(), b.bottom_right_.x()));
    auto const y1(std::min(a.top_left_.y(), b.top_left_.y()));
    auto const y2(std::max(a.bottom_right_.y(), b.bottom_right_.y()));
    return rectangle(Vector2f(x1, y1), Vector2f(x2, y2));
  }

}
