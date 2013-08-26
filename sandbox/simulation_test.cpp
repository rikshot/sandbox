#include <boost/test/unit_test.hpp>

#include "simulation.hpp"
#include "color.hpp"

BOOST_AUTO_TEST_SUITE(simulation)

BOOST_AUTO_TEST_CASE(collision) {
  sandbox::simulation simulation(200, 200);

  boost::shared_ptr<sandbox::object> const o1(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
  o1->position() = sandbox::vector(40, 40) + sandbox::vector(10, 10);

  boost::shared_ptr<sandbox::object> const o2(new sandbox::object(sandbox::shape(sandbox::rectangle(50, 50).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
  o2->position() = sandbox::vector(10, 60) + sandbox::vector(25, 25);
  o2->kinematic(true);

  simulation.objects().push_back(o1);
  simulation.objects().push_back(o2);

  simulation.step(0.01, 0.01);
  simulation.step(0.01, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()