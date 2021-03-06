#include <limits>

#include "object.hpp"

namespace sandbox {

  object::object(sandbox::shape const & shape, sandbox::material const & material) : shape_(shape), material_(material), orientation_(), angular_velocity_(), torque_(), kinematic_(false), frozen_(false) {
    mass_ = material.density() * shape.area();

    float numerator(0.0f);
    float denominator(0.0f);

    std::vector<vector> const & vertices(shape.vertices());
    for (int unsigned i(vertices.size() - 1), j(0); j < vertices.size(); i = j, ++j) {
      vector const & vertex1(vertices[i]);
      vector const & vertex2(vertices[j]);
      float const cross(vertex2.cross(vertex1));
      numerator += cross * (vertex2.dot(vertex2) + vertex2.dot(vertex1) + vertex1.dot(vertex1));
      denominator += cross;
    }

    moment_of_inertia_ = mass_ / 6.0f * (numerator / denominator);
  }

}