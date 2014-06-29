#include <vector>
#include <limits>
#include <tuple>

#include "shape.hpp"

namespace sandbox {

shape shape::core() const {
  auto const centroid(centroid());
  std::vector<vector> core(vertices_);
  std::transform(core.begin(), core.end(), core.begin(), [&](vector const & vertex) {
		return vertex - (vertex.normalize() * 2.0);
	});
  return shape(core);
}

double shape::area() const {
  double area(0.0);
  for(int unsigned i(vertices_.size() - 1), j(0); j < vertices_.size(); i = j, ++j) {
		vector const & vertex1(vertices_[i]);
		vector const & vertex2(vertices_[j]);
		area += vertex1.x() * vertex2.y() - vertex2.x() * vertex1.y();
	}
	return area / 2.0;
}

vector shape::centroid() const {
  double x(0.0);
	double y(0.0);
	for(int unsigned i(vertices_.size() - 1), j(0); j < vertices_.size(); i = j, ++j) {
		vector const & vertex1(vertices_[i]);
		vector const & vertex2(vertices_[j]);
		double const cross(vertex1.cross(vertex2));
		x += (vertex1.x() + vertex2.x()) * cross;
		y += (vertex1.y() + vertex2.y()) * cross;
	}
	double const area(6 * area());
	return vector(x / area, y / area);
}

rectangle shape::bounding_box() const {
  auto x_min(vertices_[0].x()), x_max(vertices_[0].x()), y_min(vertices_[0].y()), y_max(vertices_[0].y());
  for(vector vertex : vertices_) {
    auto const x(vertex.x());
    auto const y(vertex.y());
    if(x < x_min) x_min = x;
    else if(x > x_max) x_max = x;
    if(y < y_min) y_min = y;
    else if(y > y_max) y_max = y;
  }
  return rectangle(vector(x_min, y_min), vector(x_max, y_max));
}

bool shape::corner(vector const & vector) const {
  for(sandbox::vector vertex : vertices_) {
    if(vector == vertex) return true;
  }
  return false;
}

int unsigned shape::support(vector const & direction) const {
	int unsigned support(0);
	double maximum_dot_product(direction.dot(vertices_[0]));
	for(int unsigned i(1); i < vertices_.size(); ++i) {
		double const dot_product = direction.dot(vertices_[i]);
		if(dot_product > maximum_dot_product) {
			support = i;
			maximum_dot_product = dot_product;
		}
	}
	return support;
}

segment shape::feature(vector const & direction) const {
	int unsigned const support(support(direction));
	segment const left(vertices_[support == 0 ? vertices_.size() - 1 : support - 1], vertices_[support]);
	segment const right(vertices_[support + 1 == vertices_.size() ? 0 : support + 1], vertices_[support]);
	if(left.vector().dot(direction) < right.vector().dot(direction)) 
		return left;
	else 
		return right;
}

bool shape::intersects(shape const & shape) const {
	vector direction(shape.centroid() - centroid());

	std::vector<vector> simplex;
	simplex.push_back(vertices_[support(direction)] - shape.vertices()[shape.support(-direction)]);
	direction = -direction;

	for(;;) {
		vector const a(vertices_[support(direction)] - shape.vertices()[shape.support(-direction)]);
		if(a.dot(direction) <= 0.0) return false;
		simplex.push_back(a);
		vector const ao(-a);
		
		if(simplex.size() == 3) {
			vector const b(simplex[1]);
			vector const c(simplex[0]);
			vector const ab(b - a);
			vector const ac(c - a);
			
			vector const ab_triple(vector::triple_product_left(ac, ab, ab));
			if(ab_triple.dot(ao) >= 0.0) {
				simplex.erase(simplex.begin());
				direction = ab_triple;
			} 
			else {
				vector const ac_triple(vector::triple_product_left(ab, ac, ac));
				if(ac_triple.dot(ao) >= 0.0) {
					simplex.erase(simplex.begin() + 1);
					direction = ac_triple;
				}
				else return true;
			}
		}
		else {
			vector const b(simplex[0]);
			vector const ab(b - a);
			direction = vector::triple_product_left(ab, ao, ab);
			if(!direction) direction = ab.left();
		}
	}
}

std::tuple<vector, vector> closest_points(vector const & a1, vector const & a2, vector const & a, vector const & b1, vector const & b2, vector const & b) {
	vector const lambda(b - a);

	if(!lambda) 
		return std::make_tuple(a1, a2);
			
	double const lambda2(-lambda.dot(a) / lambda.length_squared());
	double const lambda1(1.0 - lambda2);
			
	if(lambda1 < 0.0) 
		return std::make_tuple(b1, b2);
	
	if(lambda2 < 0.0)
		return std::make_tuple(a1, a2);
			
	return std::make_tuple(a1 * lambda1 + b1 * lambda2, a2 * lambda1 + b2 * lambda2);
}

std::tuple<bool, vector, double, vector, vector> shape::distance(shape const & shape) const {
	vector direction(shape.centroid() - centroid());

	vector a1(vertices_[support(direction)]);
	vector a2(shape.vertices()[shape.support(-direction)]);
	vector a(a1 - a2);

	vector b1(vertices_[support(-direction)]);
	vector b2(shape.vertices()[shape.support(direction)]);
	vector b(b1 - b2);

	vector c1, c2, c;

	direction = segment(b, a).closest(vector());
	for(int unsigned iterations(0); iterations < 100; ++iterations) {
		direction = -direction.normalize();

		if(!direction) 
			return std::make_tuple(false, vector(), 0.0, vector(), vector());

		c1 = vertices_[support(direction)];
		c2 = shape.vertices()[shape.support(-direction)];
		c = c1 - c2;

		if(a.cross(b) * b.cross(c) > 0.0 && a.cross(b) * c.cross(a) > 0.0) 
			return std::make_tuple(false, vector(), 0.0, vector(), vector());
		
		double const projection(c.dot(direction));
		if(projection - a.dot(direction) < std::sqrt(std::numeric_limits<double>::epsilon())) {
			std::tuple<vector, vector> const closest_points(closest_points(a1, a2, a, b1, b2, b));
      return std::make_tuple(true, direction, -projection, std::get<0>(closest_points), std::get<1>(closest_points));
		}

		vector const point1(segment(a, c).closest(vector()));
		vector const point2(segment(c, b).closest(vector()));

		double const point1_length(point1.length());
		double const point2_length(point2.length());

		if(point1_length <= std::numeric_limits<double>::epsilon()) {
			std::tuple<vector, vector> const closest_points(closest_points(a1, a2, a, c1, c2, c));
      return std::make_tuple(true, direction, point1_length, std::get<0>(closest_points), std::get<1>(closest_points));
		}
		else if(point2_length <= std::numeric_limits<double>::epsilon()) {
			std::tuple<vector, vector> const closest_points(closest_points(c1, c2, c, b1, b2, b));
      return std::make_tuple(true, direction, point2_length, std::get<0>(closest_points), std::get<1>(closest_points));
		}

		if(point1_length < point2_length) {
			b1 = c1;
			b2 = c2;
			b = c;
			direction = point1;
		}
		else {
			a1 = c1;
			a2 = c2;
			a = c;
			direction = point2;
		}
	}

	std::tuple<vector, vector> const closest_points(closest_points(a1, a2, a, b1, b2, b));
  return std::make_tuple(true, direction, -c.dot(direction), std::get<0>(closest_points), std::get<1>(closest_points));
}

shape shape::transform(vector const & position, double const orientation) const {
	double const sin(std::sin(orientation));
	double const cos(std::cos(orientation));
	
	std::vector<vector> transformed_vertices(vertices_);
	std::transform(transformed_vertices.begin(), transformed_vertices.end(), transformed_vertices.begin(), [&](vector const & vertex) {
		return vector(cos * vertex.x() - sin * vertex.y(), sin * vertex.x() + cos * vertex.y()) + position;
	});

	return shape(transformed_vertices);
}

}