#include <boost/test/unit_test.hpp>

#include "quadtree.hpp"

BOOST_AUTO_TEST_SUITE(quadtree)

BOOST_AUTO_TEST_CASE(init) {
  using namespace sandbox;

  rectangle const root(vector(0.0f, 0.0f), vector(200.0f, 200.0f)); 
  sandbox::quadtree qt(root);

  /*std::shared_ptr<object> const o1(new object(shape(rectangle(vector(0.0f, 0.0f), vector(10.0f, 10.0f)).vertices()), material(1.0f, 1.0f)));
  std::shared_ptr<object> const o2(new object(shape(rectangle(vector(20.0f, 20.0f), vector(30.0f, 30.0f)).vertices()), material(1.0f, 1.0f)));
  std::shared_ptr<object> const o3(new object(shape(rectangle(vector(10.0f, 10.0f), vector(20.0f, 20.0f)).vertices()), material(1.0f, 1.0f)));
  std::shared_ptr<object> const o4(new object(shape(rectangle(vector(25.0f, 25.0f), vector(35.0f, 35.0f)).vertices()), material(1.0f, 1.0f)));
  std::shared_ptr<object> const o5(new object(shape(rectangle(vector(110.0f, 110.0f), vector(120.0f, 120.0f)).vertices()), material(1.0f, 1.0f)));

  qt.insert(o1);
  qt.insert(o2);
  qt.insert(o3);
  qt.insert(o4);
  qt.insert(o5);*/
}

BOOST_AUTO_TEST_SUITE_END()