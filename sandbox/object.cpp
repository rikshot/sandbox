#include <limits>

#include "object.hpp"
#include "misc.hpp"

namespace sandbox {

object::object(sandbox::shape const& shape, sandbox::material const& material)
    : shape_(shape)
    , material_(material)
    , orientation_()
    , angular_velocity_()
    , torque_()
    , kinematic_(false)
    , frozen_(false) {
  mass_ = material.density() * shape.area();

  float numerator(0.0);
  float denominator(0.0);

  std::vector<Vector2f> const& vertices(shape.vertices());
  for(int unsigned i(vertices.size() - 1), j(0); j < vertices.size(); i = j, ++j) {
    Vector2f const& vertex1(vertices[i]);
    Vector2f const& vertex2(vertices[j]);
    float const cross(sandbox::cross(vertex2, vertex1));
    numerator += cross * (vertex2.dot(vertex2) + vertex2.dot(vertex1) + vertex1.dot(vertex1));
    denominator += cross;
  }

  moment_of_inertia_ = mass_ / 6.0 * (numerator / denominator);
}
}
