#include <boost/test/unit_test.hpp>

#include "vector.hpp"

BOOST_AUTO_TEST_SUITE(vector)

BOOST_AUTO_TEST_CASE(cross) {
  sandbox::vector const a(10.0, 10.0);
  float const b(10.0);

  auto const c(sandbox::vector(-a.x() * b * b, -a.y() * b * b));
  auto const d(a.cross(b).cross(b));
}

BOOST_AUTO_TEST_SUITE_END()