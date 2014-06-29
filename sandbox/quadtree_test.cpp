#include <boost/test/unit_test.hpp>

#include "quadtree.hpp"

BOOST_AUTO_TEST_SUITE(quadtree)

BOOST_AUTO_TEST_CASE(init) {
  using namespace sandbox;

  rectangle const root(vector(0.0, 0.0), vector(200.0, 200.0)); 
  sandbox::quadtree qt(root);

  /*std::shared_ptr<object> const o1(new object(shape(rectangle(vector(0.0, 0.0), vector(10.0, 10.0)).vertices()), material(1.0, 1.0)));
  std::shared_ptr<object> const o2(new object(shape(rectangle(vector(20.0, 20.0), vector(30.0, 30.0)).vertices()), material(1.0, 1.0)));
  std::shared_ptr<object> const o3(new object(shape(rectangle(vector(10.0, 10.0), vector(20.0, 20.0)).vertices()), material(1.0, 1.0)));
  std::shared_ptr<object> const o4(new object(shape(rectangle(vector(25.0, 25.0), vector(35.0, 35.0)).vertices()), material(1.0, 1.0)));
  std::shared_ptr<object> const o5(new object(shape(rectangle(vector(110.0, 110.0), vector(120.0, 120.0)).vertices()), material(1.0, 1.0)));

  qt.insert(o1);
  qt.insert(o2);
  qt.insert(o3);
  qt.insert(o4);
  qt.insert(o5);*/
}

BOOST_AUTO_TEST_SUITE_END()