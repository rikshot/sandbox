#include <algorithm>
#include <set>
#include <iostream>

#include <boost\scoped_ptr.hpp>

#include "simulation.hpp"
#include "misc.hpp"
#include "contact.hpp"

namespace sandbox {

void simulation::step(double const delta_time, double const time_step) {
	time_ += delta_time;
	accumulator_ += delta_time;
	
	while(accumulator_ >= time_step) {
		contacts_.clear();
		
    std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> object_pairs;
    for(auto i : objects_) {
			for(auto j : objects_) {  
        if(i != j) {
          auto object_pair = std::make_pair(i, j);
          if(!object_pairs.count(std::make_pair(j, i))) {
            object_pairs.insert(object_pair);
          }
        }
       }
    }

    for(auto object_pair : object_pairs) {
      boost::shared_ptr<object> const a(object_pair.first);
			boost::shared_ptr<object> const b(object_pair.second);

			shape const shape1(shape::transform(a->shape().vertices(), a->position(), a->orientation()));
			shape const shape2(shape::transform(b->shape().vertices(), b->position(), b->orientation()));

			if(shape1.intersects(shape2)) {
				shape const shape1_core(shape::transform(a->shape().core(), a->position(), a->orientation()));
				shape const shape2_core(shape::transform(b->shape().core(), b->position(), b->orientation()));
					
				boost::tuple<bool, vector, double, vector, vector> const distance_data(shape1_core.distance(shape2_core));
				contact const contact(
					a, 
					b, 
					distance_data.get<3>(), 
					distance_data.get<4>(), 
					distance_data.get<1>()
				);

        if(contact.relative_velocity() < 0.0) {
          std::cout << "Contact found with RV of " << contact.relative_velocity() << '\n';
				  contacts_.push_back(contact);
        }
			}
    }

		for(auto contact : contacts_) {
		  auto a(contact.a());
			auto b(contact.b());
			auto normal(contact.normal());
			
			auto ar(contact.ap() - a->position());
			auto br(contact.bp() - b->position());
			auto arv(a->linear_velocity() + ar.cross(a->angular_velocity()));
			auto brv(b->linear_velocity() + br.cross(b->angular_velocity()));
			
			double const restitution(std::max(a->material().restitution(), b->material().restitution()));
			
			double impulse_numerator, impulse_denominator, impulse;
			if(a->kinematic()) {
				impulse_numerator = (brv * -(1.0 + restitution)).dot(normal);
				impulse_denominator = 1.0 / b->mass() + (br.cross(normal) * br.cross(normal)) / b->moment_of_inertia();
				impulse = impulse_numerator / impulse_denominator;
				
				b->linear_velocity() += normal * (impulse / b->mass());
				b->angular_velocity() += br.cross(normal * impulse) / b->moment_of_inertia();
			}
			else if(b->kinematic()) {
				impulse_numerator = (arv * -(1.0 + restitution)).dot(normal);
				impulse_denominator = 1.0 / a->mass() + (ar.cross(normal) * ar.cross(normal)) / a->moment_of_inertia();
				impulse = impulse_numerator / impulse_denominator;
				
				a->linear_velocity() += normal * (impulse / a->mass());
				a->angular_velocity() += ar.cross(normal * impulse) / a->moment_of_inertia();
			}
			else {
				vector const vab(a->linear_velocity() + ar.cross(a->angular_velocity()) - b->linear_velocity() - br.cross(b->angular_velocity()));
				
				impulse_numerator = (vab * -(1.0 + restitution)).dot(normal);
				impulse_denominator =
					1.0 / a->mass() + 1.0 / b->mass() +
					(ar.cross(normal) * ar.cross(normal)) / a->moment_of_inertia() +
					(br.cross(normal) * br.cross(normal)) / b->moment_of_inertia();
				impulse = impulse_numerator / impulse_denominator;

				a->linear_velocity() += normal * (impulse / a->mass());
				a->angular_velocity() += ar.cross(normal * impulse) / a->moment_of_inertia();

				b->linear_velocity() -= normal * (impulse / b->mass());
				b->angular_velocity() -= br.cross(normal * impulse) / b->moment_of_inertia();
			}
		}
		
		for(auto object : objects_) {
			if(!object->kinematic()) {
				object->force() = vector(0.0, 9.81);
				object->torque() = 0.0;
			}
		}
				
		integrate(time_step);
		accumulator_ -= time_step;
	}
}

static boost::tuple<vector, vector, double, double> evaluate(boost::shared_ptr<object> const & initial, double const time, double const time_step, boost::tuple<vector, vector, double, double> const & derivative) {
	return boost::make_tuple(
		initial->linear_velocity() + derivative.get<1>() * time_step, 
		initial->force(),
		initial->angular_velocity() + derivative.get<3>() * time_step,
		initial->torque()
	);
}

void simulation::integrate(double const time_step) {
	for(auto object : objects_) {
		boost::tuple<vector, vector, double, double> const a = evaluate(object, time_, 0.0, boost::tuple<vector, vector, double, double>());
		boost::tuple<vector, vector, double, double> const b = evaluate(object, time_ + time_step * 0.5, time_step * 0.5, a);
		boost::tuple<vector, vector, double, double> const c = evaluate(object, time_ + time_step * 0.5, time_step * 0.5, b);
		boost::tuple<vector, vector, double, double> const d = evaluate(object, time_ + time_step, time_step, c);

		object->position() += (a.get<0>() + (b.get<0>() + c.get<0>()) * 2.0 + d.get<0>()) * (1.0 / 6.0) * time_step;
		object->linear_velocity() += (a.get<1>() + (b.get<1>() + c.get<1>()) * 2.0 + d.get<1>()) * (1.0 / 6.0) * time_step;
		object->orientation() += (a.get<2>() + (b.get<2>() + c.get<2>()) * 2.0 + d.get<2>()) * (1.0 / 6.0) * time_step;
		object->angular_velocity() += (a.get<3>() + (b.get<3>() + c.get<3>()) * 2.0 + d.get<3>()) * (1.0 / 6.0) * time_step;
	};
}

}