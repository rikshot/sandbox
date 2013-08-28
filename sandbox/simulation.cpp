#include <algorithm>
#include <set>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <cmath>
#include <exception>

#include <boost\scoped_ptr.hpp>

#include "simulation.hpp"
#include "contact.hpp"
#include "matrix.hpp"
#include "renderer.hpp"
#include "misc.hpp"

extern boost::scoped_ptr<sandbox::renderer> renderer;
extern boost::shared_ptr<sandbox::object> object1;

namespace sandbox {

  std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> simulation::find_collisions() {
    std::lock_guard<std::mutex> const objects_lock(objects_mutex_);

    std::vector<std::pair<boost::shared_ptr<object>, rectangle const>> bounding_boxes;
    quadtree quadtree(rectangle(vector(0.0, 0.0), vector(width_, height_)));
    
    for(auto const & object : objects_) {
      rectangle const bounding_box(object->shape().transform(object->position(), object->orientation()).bounding_box());
      auto const object_with_bounding_box(std::make_pair(object, bounding_box));
      bounding_boxes.push_back(object_with_bounding_box);
      quadtree.insert(object_with_bounding_box);
    }
    
    std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> collisions;
    std::mutex collisions_mutex;  

    parallel_for(bounding_boxes.begin(), bounding_boxes.end(), [&](std::pair<boost::shared_ptr<object>, rectangle const> const & object_with_bounding_box) {
      auto const & object(object_with_bounding_box.first);
      if(!object->kinematic()) {
        auto const & bounding_box(object_with_bounding_box.second);
        std::set<boost::shared_ptr<sandbox::object>> colliders(quadtree.find(bounding_box));
        colliders.erase(object);
        std::lock_guard<std::mutex> const lock(collisions_mutex);
        for(auto const & collider : colliders) {
          if(!collisions.count(std::make_pair(collider, object))) {              
            collisions.insert(std::make_pair(object, collider));
          }
        }
      }
    });

    return collisions;
  }

  std::vector<contact> simulation::find_contacts(std::set<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> const & collisions) const {
    std::vector<contact> contacts;
    std::mutex contacts_mutex;

    std::vector<std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>>> const collision_list(collisions.begin(), collisions.end());
    
    parallel_for(collision_list.begin(), collision_list.end(), [&](std::pair<boost::shared_ptr<object>, boost::shared_ptr<object>> const & collision) {
      auto const a(collision.first);
			auto const b(collision.second);
      
      shape const a_shape(a->shape().transform(a->position(), a->orientation()));
			shape const b_shape(b->shape().transform(b->position(), b->orientation()));

			if(a_shape.intersects(b_shape)) {
        shape const a_core(a->shape().core().transform(a->position(), a->orientation()));
        shape const b_core(b->shape().core().transform(b->position(), b->orientation()));
					
				boost::tuple<bool, vector, double, vector, vector> const distance_data(b_core.distance(a_core));

        auto const & normal(distance_data.get<1>());
        
        auto ap(distance_data.get<4>());
        auto bp(distance_data.get<3>());
        
        auto const a_feature(a_shape.feature(-normal));
        auto const b_feature(b_shape.feature(normal));

        auto const a_segment(segment(a_feature.closest(b_feature.a()), a_feature.closest(b_feature.b())));
        auto const b_segment(segment(b_feature.closest(a_feature.a()), b_feature.closest(a_feature.b())));

        if(a_segment.vector().parallel(b_segment.vector())) {
          contact const contact(
            a,
            b,
            a_segment.middle(),
            b_segment.middle(),
            normal
          );
          std::lock_guard<std::mutex> lock(contacts_mutex);
          contacts.push_back(contact);
          /*contact const contact1(
            a,
            b,
            a_feature.closest(b_feature.a()),
            a_feature.closest(b_feature.b()),
            normal
          );
          contact const contact2(
            a,
            b,
            b_feature.closest(a_feature.a()),
            b_feature.closest(a_feature.b()),
            normal
          );
          if(contact1.relative_velocity() > 0.0) {
            std::lock_guard<std::mutex> lock(contacts_mutex);
            contacts.push_back(contact1);
          }
          if(contact2.relative_velocity() > 0.0) {
            std::lock_guard<std::mutex> lock(contacts_mutex);
            contacts.push_back(contact2);
          }*/
        } else {
          if(a_core.corner(ap)) {
            ap += (ap - a->position()).normalize() * 2.0;
            bp = b_feature.closest(ap);
          } else if(b_core.corner(bp)) {
            bp += (bp - b->position()).normalize() * 2.0;
            ap = a_feature.closest(bp);
          }

          contact const contact(
					  a, 
					  b, 
            ap, 
            bp, 
					  normal
				  );
          std::lock_guard<std::mutex> lock(contacts_mutex);
          contacts.push_back(contact);
        }
			}
    });

    return contacts;
  }

  void simulation::step(double const delta_time, double const time_step) {
	  time_ += delta_time;
	  accumulator_ += delta_time;

	  while(accumulator_ >= time_step) {
      for(auto object : objects_) {
			  if(!object->kinematic()) {
          object->force() = vector(0.0, 9.81);
          object->torque() = 0.0;
			  }
		  }

      auto const collisions(find_collisions());
      auto contacts(find_contacts(collisions));
      if(!contacts.empty()) {
        contacts.erase(std::remove_if(contacts.begin(), contacts.end(), [](contact const & contact) {
          return contact.relative_velocity() < 0.0;
        }), contacts.end());

        if(!contacts.empty()) {
          resolve_collisions(contacts);

          contacts.erase(std::remove_if(contacts.begin(), contacts.end(), [](contact const & contact) {
            return contact.relative_velocity() < 0.0;
          }), contacts.end());

          if(!contacts.empty()) {
            resolve_contacts(contacts);
          }
        }
      } 
			
		  integrate(time_step);
		  accumulator_ -= time_step;
	  }
  }

  void simulation::resolve_collisions(std::vector<contact> const & contacts) {
    std::lock_guard<std::mutex> const objects_lock(objects_mutex_);

    parallel_for(contacts.begin(), contacts.end(), [&](contact const & contact) {
      auto const & a(contact.a());
			auto const & b(contact.b());
			auto const & normal(contact.normal());
			
			auto const ar(contact.ap() - a->position());
			auto const br(contact.bp() - b->position());
			auto const arv(a->linear_velocity() + ar.cross(a->angular_velocity()));
			auto const brv(b->linear_velocity() + br.cross(b->angular_velocity()));
			
			double const restitution(std::max(a->material().restitution(), b->material().restitution()));
			
			double impulse_numerator, impulse_denominator, impulse;
			if(a->kinematic()) {
				impulse_numerator = (brv * -(1.0 + restitution)).dot(normal);
				impulse_denominator = 1.0 / b->mass() + (br.cross(normal) * br.cross(normal)) / b->moment_of_inertia();
				impulse = impulse_numerator / impulse_denominator;
				
        std::lock_guard<std::mutex> const lock(b->mutex());
				b->linear_velocity() += normal * (impulse / b->mass());
				b->angular_velocity() += br.cross(normal * impulse) / b->moment_of_inertia();
			}
			else if(b->kinematic()) {
				impulse_numerator = (arv * -(1.0 + restitution)).dot(normal);
				impulse_denominator = 1.0 / a->mass() + (ar.cross(normal) * ar.cross(normal)) / a->moment_of_inertia();
				impulse = impulse_numerator / impulse_denominator;
				
        std::lock_guard<std::mutex> const lock(a->mutex());
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

        std::lock(a->mutex(), b->mutex());

				a->linear_velocity() += normal * (impulse / a->mass());
				a->angular_velocity() += ar.cross(normal * impulse) / a->moment_of_inertia();

				b->linear_velocity() -= normal * (impulse / b->mass());
				b->angular_velocity() -= br.cross(normal * impulse) / b->moment_of_inertia();

        a->mutex().unlock();
        b->mutex().unlock();
			}
    });
  }

  void simulation::resolve_contacts(std::vector<contact> const & contacts) {
    auto const n(contacts.size());
    matrix<> A(n, n);
    for(unsigned int i(0); i < n; ++i) {
      auto const & contact_i(contacts[i]);
      auto const & i_a(contact_i.a());
      auto const & i_b(contact_i.b());
      auto const & i_normal(contact_i.normal());

      auto const i_ar(contact_i.ap() - i_a->position());
	    auto const i_br(contact_i.bp() - i_b->position());

      for(unsigned int j(0); j < n; ++j) {
        auto const & contact_j(contacts[j]);
        auto const & j_a(contact_j.a());
        auto const & j_b(contact_j.b());
        auto const & j_normal(contact_j.normal());

        auto const j_ar(contact_j.ap() - j_a->position());
		    auto const j_br(contact_j.bp() - j_b->position());

        if(i_a == j_a) {
          A(i, j) += i_normal.dot(j_normal / i_a->mass() + i_ar.cross(j_ar.cross(j_normal)) / i_a->moment_of_inertia());
        }
        if(i_a == j_b) {
          A(i, j) -= i_normal.dot(j_normal / i_a->mass() + i_ar.cross(j_br.cross(j_normal)) / i_a->moment_of_inertia());
        }
        if(!i_b->kinematic() && i_b == j_a) {
          A(i, j) -= i_normal.dot(j_normal / i_b->mass() + i_br.cross(j_ar.cross(j_normal)) / i_b->moment_of_inertia());
        }
        if(!i_b->kinematic() && i_b == j_b) {
          A(i, j) += i_normal.dot(j_normal / i_b->mass() + i_br.cross(j_br.cross(j_normal)) / i_b->moment_of_inertia());
        }
      }
    }

    matrix<> B(n);
    for(unsigned int i(0); i < n; ++i) {
      auto const & contact(contacts[i]);
      auto const & a(contact.a());
      auto const & b(contact.b());
      auto const & normal(contact.normal());

      auto const ar(contact.ap() - a->position());
			auto const br(contact.bp() - b->position());
			auto const arv(a->linear_velocity() + ar.cross(a->angular_velocity()));
			auto const brv(b->linear_velocity() + br.cross(b->angular_velocity()));

      if(!b->kinematic()) {
        //B(i) += 2.0 * b->angular_velocity() * normal.dot(arv - brv);
        B(i) += 2.0 * normal.cross(b->angular_velocity()).dot(arv - brv);
      }
      
      B(i) += normal.dot(a->force() / a->mass() + ar.cross(a->torque() / a->moment_of_inertia()) + ar.cross(a->angular_velocity()).cross(a->angular_velocity()));
      B(i) -= normal.dot(b->force() / b->mass() + br.cross(b->torque() / b->moment_of_inertia()) + br.cross(b->angular_velocity()).cross(b->angular_velocity()));
   
      /*auto const af(a->force() / a->mass());
      auto const bf(b->force() / b->mass());

      auto const at(a->torque() / a->moment_of_inertia());
      auto const bt(b->torque() / b->moment_of_inertia());
      
      auto const aw2(a->angular_velocity() * a->angular_velocity());
      auto const bw2(b->angular_velocity() * b->angular_velocity());

      B(i) += normal.dot(vector(af.x() - aw2 * ar.x() - at * ar.y(), af.y() - aw2 * ar.y() + at * ar.x()));
      B(i) -= normal.dot(vector(bf.x() - bw2 * br.x() - bt * br.y(), bf.y() - bw2 * br.y() + bt * br.x()));*/
    }

    matrix<> f(n);
    /*for(unsigned int c(0); c < 100; ++c) {
      for(unsigned int i(0); i < n; ++i) {
        auto q(B(i));
        for(unsigned int j(0); j < n; ++j) {
          if(j != i) {
            q += A(i, j) * f(j);
          }
        }
        if(q >= 0.0) {
          f(i) = 0.0;
        } else {
          f(i) -= q / A(i, i);
        }
      }
    }*/
  
    matrix<> a(B);
    std::vector<bool> C(n, false);
    std::vector<bool> NC(n, false);

    auto maxStep([&](matrix<> & delta_f, matrix<> & delta_a, unsigned const d) {
      double s(std::numeric_limits<double>::infinity());
      int j(-1);

      if(delta_a(d) > 0.0) {
        j = d;
        s = -a(d) / delta_a(d);
      }

      for(unsigned i(0); i < n; ++i) {
        if(C[i] && delta_f(i) < 0.0) {
          double sPrime(-f(i) / delta_f(i));
          if(sPrime < s) {
            s = sPrime;
            j = i;
          }
        }
      }

      for(unsigned i(0); i < n; ++i) {
        if(NC[i] && delta_a(i) < 0.0) {
          double sPrime(-a(i) / delta_a(i));
          if(sPrime < s) {
            s = sPrime;
            j = i;
          }
        }
      }

      if(s < 0.0) {
        throw std::runtime_error("stepSize is negative!");
      }

      return std::make_pair(j, s);
    });

    auto fdirection([&](matrix<> & delta_f, unsigned const d) {
      delta_f(d) = 1.0;

      unsigned c(0);
      for(unsigned i(0); i < n; ++i) {
        if(C[i]) {
          ++c;
        }
      }

      if(c) {
        matrix<> Acc(c, c + 1);

        int p(0);
        for(unsigned i(0); i < n; ++i) {
          if(C[i]) {
            int q(0);
            for(unsigned j(0); j < n; ++j) {
              if(C[j]) {
                Acc(p, q) = A(i, j);
                ++q;
              }
            }

            Acc(p, c) = -A(i, d);
            ++p;
          }
        }

        matrix<> x(Acc.solve());

        p = 0;
        for(unsigned i(0); i < n; ++i) {
          if(C[i]) {
            delta_f(i) = x(p++);
          }
        }
      }
    });   

    for(unsigned d(0); d < n; ++d) {
      if(a(d) >= 0.0) NC[d] = true;

      while(a(d) < 0.0) {
        matrix<> delta_f(n);
        fdirection(delta_f, d);

        matrix<> delta_a(A * delta_f);

        for(unsigned i(0); i < n; ++i) {
          if(C[i] && std::abs(delta_a(i)) > 10e-10) {
            throw std::runtime_error("fdirection failed!");
          }
        }

        unsigned j;
        double stepSize;
        std::tie(j, stepSize) = maxStep(delta_f, delta_a, d);

        for(unsigned i(0); i < n; ++i) {
          f(i) += stepSize * delta_f(i);
          a(i) += stepSize * delta_a(i);

          if(f(i) < 0.0 && -10e-10 < f(i)) {
            f(i) = 0.0;
          }

          if(a(i) < 0.0 && -10e-10 < a(i)) {
            a(i) = 0.0;
          }

          if((NC[i] || C[i]) && a(i) < 0.0) {
            throw std::runtime_error("Acceleration cannot be negative!");
          }

          if(f(i) < 0.0) {
            throw std::runtime_error("Reaction force cannot be negative!");
          }
        }

        if(C[j]) {
          C[j] = false;
          NC[j] = true;
        } else if(NC[j]) {
          NC[j] = false;
          C[j] = true;
        } else {
          C[j] = true;
          break;
        }
      }
    }

    for(unsigned int i(0); i < n; ++i) {
      auto & contact(contacts[i]);
      auto & a(contact.a());
      auto & b(contact.b());
      auto const & normal(contact.normal());

      auto const force(f(i));

      auto const ar(contact.ap() - a->position());
			auto const br(contact.bp() - b->position());

      /*if(!a->kinematic()) {
        a->linear_velocity() += normal * (force / a->mass());
        a->angular_velocity() += ar.cross(normal * force) / a->moment_of_inertia();
      }
      if(!b->kinematic()) {
        b->linear_velocity() -= normal * (force / b->mass());
        b->angular_velocity() -= br.cross(normal * force) / b->moment_of_inertia();
      }*/
      if(!a->kinematic()) {
        a->force() += normal * (force / a->mass());
        a->torque() += ar.cross(normal * force) / a->moment_of_inertia();
      }
      if(!b->kinematic()) {
        b->force() -= normal * (force / b->mass());
        b->torque() -= br.cross(normal * force) / b->moment_of_inertia();
      }
    }
  }

  boost::tuple<vector, vector, double, double> simulation::evaluate(boost::shared_ptr<object> const & initial, double const time, double const time_step, boost::tuple<vector, vector, double, double> const & derivative) const {
	  return boost::make_tuple(initial->linear_velocity() + derivative.get<1>() * time_step,  initial->force(), initial->angular_velocity() + derivative.get<3>() * time_step, initial->torque());
  }

  void simulation::integrate(double const time_step) {
    std::lock_guard<std::mutex> lock(objects_mutex_);
    
    parallel_for(objects_.begin(), objects_.end(), [&](boost::shared_ptr<object> const & object) {
      if(!object->kinematic()) {
		    auto const a(evaluate(object, time_, 0.0, boost::tuple<vector, vector, double, double>()));
		    auto const b(evaluate(object, time_ + time_step * 0.5, time_step * 0.5, a));
		    auto const c(evaluate(object, time_ + time_step * 0.5, time_step * 0.5, b));
		    auto const d(evaluate(object, time_ + time_step, time_step, c));

		    object->position() += (a.get<0>() + (b.get<0>() + c.get<0>()) * 2.0 + d.get<0>()) * (1.0 / 6.0) * time_step;
		    object->linear_velocity() += (a.get<1>() + (b.get<1>() + c.get<1>()) * 2.0 + d.get<1>()) * (1.0 / 6.0) * time_step;
		    object->orientation() += (a.get<2>() + (b.get<2>() + c.get<2>()) * 2.0 + d.get<2>()) * (1.0 / 6.0) * time_step;
		    object->angular_velocity() += (a.get<3>() + (b.get<3>() + c.get<3>()) * 2.0 + d.get<3>()) * (1.0 / 6.0) * time_step;
      }
    });
  }

}