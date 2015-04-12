#include <boost/test/unit_test.hpp>

#include "vector.hpp"

BOOST_AUTO_TEST_SUITE(vector)

BOOST_AUTO_TEST_CASE(cross) {
  sandbox::vector const a(10.0f, 10.0f);
  float const b(10.0f);

  auto const c(sandbox::vector(-a.x() * b * b, -a.y() * b * b));
  auto const d(a.cross(b).cross(b));
}

BOOST_AUTO_TEST_SUITE_END()