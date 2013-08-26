#include <boost/test/unit_test.hpp>

#include "matrix.hpp"

BOOST_AUTO_TEST_SUITE(matrix)

BOOST_AUTO_TEST_CASE(construction) {
  using namespace sandbox;

  sandbox::matrix<> foo(2, 2);
  BOOST_CHECK(foo(0, 0) == 0.0);

  foo(1, 0) = 1.0;
  BOOST_CHECK(foo(1, 0) == 1.0);

  auto const bar(foo * 2);
  BOOST_CHECK(bar(1, 0) == 2.0);

  auto const foobar(bar.transpose());
  BOOST_CHECK(foobar(0, 1) == 2.0);

  sandbox::matrix<> a(3, 2);
  sandbox::matrix<> b(2, 3);

  a(0, 0) = 2.0;
  b(0, 1) = 4.0;

  auto const c(a * b);
  BOOST_CHECK(c(0, 1) == 8.0);
}

BOOST_AUTO_TEST_SUITE_END()