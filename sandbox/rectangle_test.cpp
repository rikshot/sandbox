#include <boost/test/unit_test.hpp>

#include "rectangle.hpp"

BOOST_AUTO_TEST_SUITE(rectangle)

BOOST_AUTO_TEST_CASE(contains_vertex) {
  sandbox::rectangle const r1(sandbox::vector(10.0, 10.0), sandbox::vector(20.0, 20.0));
  sandbox::vector const v1(15.0, 15.0);
  sandbox::vector const v2(30.0, 30.0);

  BOOST_CHECK(r1.contains(v1));
  BOOST_CHECK(!r1.contains(v2));
}

BOOST_AUTO_TEST_CASE(contains_rectangle) {
  sandbox::rectangle const r1(sandbox::vector(10.0, 10.0), sandbox::vector(20.0, 20.0));
  sandbox::rectangle const r2(sandbox::vector(15.0, 15.0), sandbox::vector(25.0, 25.0));
  sandbox::rectangle const r3(sandbox::vector(30.0, 30.0), sandbox::vector(40.0, 40.0));
  sandbox::rectangle const r4(sandbox::vector(0.0, 0.0), sandbox::vector(50.0, 50.0));

  BOOST_CHECK(r1.contains(r1));
  BOOST_CHECK(!r1.contains(r2));
  BOOST_CHECK(!r1.contains(r3));
  BOOST_CHECK(!r1.contains(r4));
  BOOST_CHECK(r4.contains(r1));
}

BOOST_AUTO_TEST_CASE(overlaps) {
  sandbox::rectangle const r1(sandbox::vector(10.0, 10.0), sandbox::vector(20.0, 20.0));
  sandbox::rectangle const r2(sandbox::vector(15.0, 15.0), sandbox::vector(25.0, 25.0));
  sandbox::rectangle const r3(sandbox::vector(30.0, 30.0), sandbox::vector(40.0, 40.0));
  sandbox::rectangle const r4(sandbox::vector(0.0, 0.0), sandbox::vector(50.0, 50.0));

  BOOST_CHECK(r1.overlaps(r1));
  BOOST_CHECK(r1.overlaps(r2));
  BOOST_CHECK(!r1.overlaps(r3));
  BOOST_CHECK(r1.overlaps(r4));
  BOOST_CHECK(r4.overlaps(r1));
}

BOOST_AUTO_TEST_SUITE_END()