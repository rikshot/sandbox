#include "vector.hpp"
#include "shape.hpp"
#include "misc.hpp"

#define BOOST_TEST_MODULE Shape
#include <boost\test\unit_test.hpp>

sandbox::shape const shape1(sandbox::rectangle(25, 25));
sandbox::shape const core1(shape1.core());

sandbox::shape const shape2(sandbox::shape(sandbox::shape::transform(sandbox::rectangle(25, 25), sandbox::vector(20, 0), 45)));
sandbox::shape const core2(shape2.core());

sandbox::shape const shape3(sandbox::shape(sandbox::shape::transform(sandbox::rectangle(25, 25), sandbox::vector(50, 0), 0)));
sandbox::shape const core3(shape3.core());

BOOST_AUTO_TEST_CASE(intersection) {
	BOOST_CHECK(shape1.intersects(shape2));
	BOOST_CHECK(!shape1.intersects(shape3));
}

BOOST_AUTO_TEST_CASE(distance) {
	BOOST_CHECK_EQUAL(shape1.distance(shape3).get<2>(), 25);
	boost::tuple<bool, sandbox::vector, float, sandbox::vector, sandbox::vector> const distance_data(core1.distance(core2));
	int i = 0;
}