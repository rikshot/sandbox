#include <vector>
#include <limits>
#include <tuple>

#include "shape.hpp"
#include "misc.hpp"

namespace sandbox {

shape shape::core() const {
  std::vector<Vector2f> core(vertices_);
  std::transform(core.begin(), core.end(), core.begin(), [&](Vector2f const& vertex) {
    return vertex - (vertex.normalized() * 4.0);
  });
  return shape(core);
}

float shape::area() const {
  float area(0.0);
  for(int unsigned i(vertices_.size() - 1), j(0); j < vertices_.size(); i = j, ++j) {
    Vector2f const& vertex1(vertices_[i]);
    Vector2f const& vertex2(vertices_[j]);
    area += vertex1.x() * vertex2.y() - vertex2.x() * vertex1.y();
  }
  return area / 2.0;
}

Vector2f shape::centroid() const {
  float x(0.0);
  float y(0.0);
  for(int unsigned i(vertices_.size() - 1), j(0); j < vertices_.size(); i = j, ++j) {
    Vector2f const& vertex1(vertices_[i]);
    Vector2f const& vertex2(vertices_[j]);
    float const cross(sandbox::cross(vertex1, vertex2));
    x += (vertex1.x() + vertex2.x()) * cross;
    y += (vertex1.y() + vertex2.y()) * cross;
  }
  float const area(6 * this->area());
  return Vector2f(x / area, y / area);
}

rectangle shape::bounding_box() const {
  auto x_min(vertices_[0].x()), x_max(vertices_[0].x()), y_min(vertices_[0].y()), y_max(vertices_[0].y());
  for(Vector2f vertex : vertices_) {
    auto const x(vertex.x());
    auto const y(vertex.y());
    if(x < x_min)
      x_min = x;
    else if(x > x_max)
      x_max = x;
    if(y < y_min)
      y_min = y;
    else if(y > y_max)
      y_max = y;
  }
  return rectangle(Vector2f(x_min, y_min), Vector2f(x_max, y_max));
}

bool shape::corner(Vector2f const& vector) const {
  for(Vector2f vertex : vertices_) {
    if(vector == vertex)
      return true;
  }
  return false;
}

int unsigned shape::support(Vector2f const& direction) const {
  int unsigned support(0);
  float maximum_dot_product(direction.dot(vertices_[0]));
  for(int unsigned i(1); i < vertices_.size(); ++i) {
    float const dot_product = direction.dot(vertices_[i]);
    if(dot_product > maximum_dot_product) {
      support = i;
      maximum_dot_product = dot_product;
    }
  }
  return support;
}

segment shape::feature(Vector2f const& direction) const {
  int unsigned const support(this->support(direction));
  segment const left(vertices_[support == 0 ? vertices_.size() - 1 : support - 1], vertices_[support]);
  segment const right(vertices_[support + 1 == vertices_.size() ? 0 : support + 1], vertices_[support]);
  if(left.getVector().dot(direction) < right.getVector().dot(direction))
    return left;
  else
    return right;
}

bool shape::intersects(shape const& shape) const {
  Vector2f direction(shape.centroid() - centroid());

  std::vector<Vector2f> simplex;
  simplex.push_back(vertices_[support(direction)] - shape.vertices()[shape.support(-direction)]);
  direction = -direction;

  for(;;) {
    Vector2f const a(vertices_[support(direction)] - shape.vertices()[shape.support(-direction)]);
    if(a.dot(direction) <= 0.0)
      return false;
    simplex.push_back(a);
    Vector2f const ao(-a);

    if(simplex.size() == 3) {
      Vector2f const b(simplex[1]);
      Vector2f const c(simplex[0]);
      Vector2f const ab(b - a);
      Vector2f const ac(c - a);

      Vector2f const ab_triple(triple_product_left(ac, ab, ab));
      if(ab_triple.dot(ao) >= 0.0) {
        simplex.erase(simplex.begin());
        direction = ab_triple;
      } else {
        Vector2f const ac_triple(triple_product_left(ab, ac, ac));
        if(ac_triple.dot(ao) >= 0.0) {
          simplex.erase(simplex.begin() + 1);
          direction = ac_triple;
        } else
          return true;
      }
    } else {
      Vector2f const b(simplex[0]);
      Vector2f const ab(b - a);
      direction = triple_product_left(ab, ao, ab);
      if(!direction.any())
        direction = Vector2f(ab.y(), -ab.x());
    }
  }
}

std::tuple<Vector2f, Vector2f> get_closest_points(Vector2f const& a1,
                                                  Vector2f const& a2,
                                                  Vector2f const& a,
                                                  Vector2f const& b1,
                                                  Vector2f const& b2,
                                                  Vector2f const& b) {
  Vector2f const lambda(b - a);

  if(!lambda.any())
    return std::make_tuple(a1, a2);

  float const lambda2(-lambda.dot(a) / lambda.squaredNorm());
  float const lambda1(1.0 - lambda2);

  if(lambda1 < 0.0)
    return std::make_tuple(b1, b2);

  if(lambda2 < 0.0)
    return std::make_tuple(a1, a2);

  return std::make_tuple(a1 * lambda1 + b1 * lambda2, a2 * lambda1 + b2 * lambda2);
}

std::tuple<bool, Vector2f, float, Vector2f, Vector2f> shape::distance(shape const& shape) const {
  Vector2f direction(shape.centroid() - centroid());

  Vector2f a1(vertices_[support(direction)]);
  Vector2f a2(shape.vertices()[shape.support(-direction)]);
  Vector2f a(a1 - a2);

  Vector2f b1(vertices_[support(-direction)]);
  Vector2f b2(shape.vertices()[shape.support(direction)]);
  Vector2f b(b1 - b2);

  Vector2f c1, c2, c;

  direction = segment(b, a).closest(Vector2f());
  for(int unsigned iterations(0); iterations < 10; ++iterations) {
    direction = -direction.normalized();

    if(!direction.any())
      return std::make_tuple(false, Vector2f(), 0.0, Vector2f(), Vector2f());

    c1 = vertices_[support(direction)];
    c2 = shape.vertices()[shape.support(-direction)];
    c = c1 - c2;

    if(sandbox::cross(a, b) * sandbox::cross(b, c) > 0.0 && sandbox::cross(a, b) * sandbox::cross(c, a) > 0.0)
      return std::make_tuple(false, Vector2f(), 0.0, Vector2f(), Vector2f());

    float const projection(c.dot(direction));
    if(projection - a.dot(direction) < std::sqrt(std::numeric_limits<float>::epsilon())) {
      std::tuple<Vector2f, Vector2f> const closest_points(get_closest_points(a1, a2, a, b1, b2, b));
      return std::make_tuple(true, direction, -projection, std::get<0>(closest_points), std::get<1>(closest_points));
    }

    Vector2f const point1(segment(a, c).closest(Vector2f()));
    Vector2f const point2(segment(c, b).closest(Vector2f()));

    float const point1_length(point1.norm());
    float const point2_length(point2.norm());

    if(point1_length <= std::numeric_limits<float>::epsilon()) {
      std::tuple<Vector2f, Vector2f> const closest_points(get_closest_points(a1, a2, a, c1, c2, c));
      return std::make_tuple(true, direction, point1_length, std::get<0>(closest_points), std::get<1>(closest_points));
    } else if(point2_length <= std::numeric_limits<float>::epsilon()) {
      std::tuple<Vector2f, Vector2f> const closest_points(get_closest_points(c1, c2, c, b1, b2, b));
      return std::make_tuple(true, direction, point2_length, std::get<0>(closest_points), std::get<1>(closest_points));
    }

    if(point1_length < point2_length) {
      b1 = c1;
      b2 = c2;
      b = c;
      direction = point1;
    } else {
      a1 = c1;
      a2 = c2;
      a = c;
      direction = point2;
    }
  }

  std::tuple<Vector2f, Vector2f> const closest_points(get_closest_points(a1, a2, a, b1, b2, b));
  return std::make_tuple(true, direction, -c.dot(direction), std::get<0>(closest_points), std::get<1>(closest_points));
}

shape shape::transform(Vector2f const& position, float const orientation) const {
  float const sin(std::sin(orientation));
  float const cos(std::cos(orientation));

  std::vector<Vector2f> transformed_vertices(vertices_);
  std::transform(transformed_vertices.begin(),
                 transformed_vertices.end(),
                 transformed_vertices.begin(),
                 [&](Vector2f const& vertex) {
    return Vector2f(cos * vertex.x() - sin * vertex.y(), sin * vertex.x() + cos * vertex.y()) + position;
  });

  return shape(transformed_vertices);
}
}
