#include <boost/test/unit_test.hpp>

#include "rectangle.hpp"

BOOST_AUTO_TEST_SUITE(rectangle)

BOOST_AUTO_TEST_CASE(contains_vertex) {
  sandbox::rectangle const r1(sandbox::vector(10.0f, 10.0f), sandbox::vector(20.0f, 20.0f));
  sandbox::vector const v1(15.0f, 15.0f);
  sandbox::vector const v2(30.0f, 30.0f);

  BOOST_CHECK(r1.contains(v1));
  BOOST_CHECK(!r1.contains(v2));
}

BOOST_AUTO_TEST_CASE(contains_rectangle) {
  sandbox::rectangle const r1(sandbox::vector(10.0f, 10.0f), sandbox::vector(20.0f, 20.0f));
  sandbox::rectangle const r2(sandbox::vector(15.0f, 15.0f), sandbox::vector(25.0f, 25.0f));
  sandbox::rectangle const r3(sandbox::vector(30.0f, 30.0f), sandbox::vector(40.0f, 40.0f));
  sandbox::rectangle const r4(sandbox::vector(0.0f, 0.0f), sandbox::vector(50.0f, 50.0f));

  BOOST_CHECK(r1.contains(r1));
  BOOST_CHECK(!r1.contains(r2));
  BOOST_CHECK(!r1.contains(r3));
  BOOST_CHECK(!r1.contains(r4));
  BOOST_CHECK(r4.contains(r1));
}

BOOST_AUTO_TEST_CASE(overlaps) {
  sandbox::rectangle const r1(sandbox::vector(10.0f, 10.0f), sandbox::vector(20.0f, 20.0f));
  sandbox::rectangle const r2(sandbox::vector(15.0f, 15.0f), sandbox::vector(25.0f, 25.0f));
  sandbox::rectangle const r3(sandbox::vector(30.0f, 30.0f), sandbox::vector(40.0f, 40.0f));
  sandbox::rectangle const r4(sandbox::vector(0.0f, 0.0f), sandbox::vector(50.0f, 50.0f));

  BOOST_CHECK(r1.overlaps(r1));
  BOOST_CHECK(r1.overlaps(r2));
  BOOST_CHECK(!r1.overlaps(r3));
  BOOST_CHECK(r1.overlaps(r4));
  BOOST_CHECK(r4.overlaps(r1));
}

BOOST_AUTO_TEST_SUITE_END()