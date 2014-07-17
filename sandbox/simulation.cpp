#include <algorithm>
#include <set>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <cmath>
#include <exception>
#include <memory>

#include "simulation.hpp"
#include "contact.hpp"
#include "matrix.hpp"
#include "renderer.hpp"
#include "misc.hpp"

extern std::shared_ptr<sandbox::renderer> renderer;
extern std::shared_ptr<sandbox::object> object1;

namespace sandbox {

  void simulation::update_world_shapes() {
    world_shapes_.clear();
    parallel_for_range(objects_.begin(), objects_.end(), [&](std::shared_ptr<object> const & object) {
      auto const world_shape(
        std::make_pair(
          object,
          object->shape().transform(
            object->position(),
            object->orientation()
          )
        )
      );
      std::lock_guard<std::mutex> lock(world_shapes_mutex_);
      world_shapes_.emplace(world_shape);
    });
  }

  void simulation::update_bounding_boxes() {
    bounding_boxes_.clear();
    parallel_for_range(objects_.begin(), objects_.end(), [&](object_t const & object) {
      auto const bounding_box(
        std::make_pair(
          object,
          world_shapes_[object].bounding_box()
        )
      );
      std::lock_guard<std::mutex> lock(bounding_boxes_mutex_);
      bounding_boxes_.emplace(bounding_box);
    });
  }

  void simulation::update_quadtree() {
    quadtree_.clear();
    for(auto const & object : objects_) {
      quadtree_.insert(
        std::make_pair(
          object,
          bounding_boxes_[object]
        )
      );
    }
  }

  void simulation::find_collisions() {
    collisions_.clear();
    parallel_for_range(objects_.begin(), objects_.end(), [&](object_t const & object) {
      if(!object->kinematic()) {
        std::unordered_set<object_t> colliders(quadtree_.find(bounding_boxes_[object]));
        colliders.erase(object);
        std::lock_guard<std::mutex> const lock(collisions_mutex_);
        for(auto const & collider : colliders) {
          if(!object->frozen() || !collider->frozen()) {
            if(!collisions_.count(collider) || !collisions_[collider].count(object)) {
              collisions_[object].emplace(collider);
            }
          }
        }
      }
    });
  }

  void simulation::find_islands() {
    std::unordered_map<object_t, std::shared_ptr<std::set<std::pair<object_t, object_t>>>> islands;
    std::for_each(collisions_.begin(), collisions_.end(), [&](auto const & collision) {
      auto & object(collision.first);
      auto & colliders(collision.second);

      std::for_each(colliders.begin(), colliders.end(), [&](auto const & collider) {
        std::shared_ptr<std::set<std::pair<object_t, object_t>>> island;

        if(islands.count(object)) {
          island = islands[object];
        } else if(islands.count(collider)) {
          island = islands[collider];
        } else {
          island = std::make_shared<std::set<std::pair<object_t, object_t>>>();
        }

        if(!island->count(std::make_pair(object, collider)) && !island->count(std::make_pair(collider, object))) {
          island->emplace(std::make_pair(object, collider));
          islands[object] = island;
          islands[collider] = island;
        }
      });
    });

    islands_.clear();
    for(auto const & island : islands) {
      islands_.emplace(island.second);
    }
  }

  void simulation::find_contacts() {
    contacts_ = std::vector<std::vector<contact>>(islands_.size());

    parallel_for_index(islands_.begin(), islands_.end(), [&](auto const & island, std::size_t const index) {
      std::vector<std::pair<object_t, object_t>> const collision_list(island->begin(), island->end());

      parallel_for_range(collision_list.begin(), collision_list.end(), [&, index](auto const & collision) {
        auto const & a(collision.first);
        auto const & b(collision.second);

        shape const & a_shape(world_shapes_[a]);
        shape const & b_shape(world_shapes_[b]);

        if(a_shape.intersects(b_shape)) {
          shape const a_core(a->shape().core().transform(a->position(), a->orientation()));
          shape const b_core(b->shape().core().transform(b->position(), b->orientation()));

          std::tuple<bool, vector, double, vector, vector> const distance_data(b_core.distance(a_core));

          auto const & normal(std::get<1>(distance_data));

          auto ap(std::get<4>(distance_data));
          auto bp(std::get<3>(distance_data));

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
            if(contact.relative_velocity() >= 0.0) {
              std::lock_guard<std::mutex> lock(contacts_mutex_);
              contacts_[index].emplace_back(contact);
            }
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
            if(contact.relative_velocity() >= 0.0) {
              std::lock_guard<std::mutex> lock(contacts_mutex_);
              contacts_[index].emplace_back(contact);
            }
          }
        }
      });
    });
  }

  void simulation::step(double const delta_time, double const time_step) {
	  time_ += delta_time;
	  accumulator_ += delta_time;

	  while(accumulator_ >= time_step) {
      for(auto object : objects_) {
        if(!object->kinematic() && !object->frozen()) {
          object->force() = vector(0.0, 9.81);
          object->torque() = 0.0;
			  }
		  }

      update_world_shapes();
      update_bounding_boxes();
      update_quadtree();

      find_collisions();
      find_islands();
      find_contacts();

      if(!contacts_.empty()) {
        resolve_collisions();

        for(auto & island : contacts_) {
          island.erase(std::remove_if(island.begin(), island.end(), [](contact const & contact) {
            return contact.relative_velocity() < 0.0;
          }), island.end());
        }

        resolve_contacts();
      } 
			
		  integrate(time_step);
		  accumulator_ -= time_step;
	  }
  }

  void simulation::resolve_collisions() {
    parallel_for(contacts_.begin(), contacts_.end(), [&](auto const & island) {
      parallel_for_range(island.begin(), island.end(), [&](contact const & contact) {
        auto const & a(contact.a());
        auto const & b(contact.b());
        auto const & normal(contact.normal());

        auto const ar(contact.ap() - a->position());
        auto const br(contact.bp() - b->position());

        double const restitution(std::max(a->material().restitution(), b->material().restitution()));

        double impulse_numerator, impulse_denominator, impulse;
        if(a->kinematic()) {
          std::lock_guard<std::mutex> const lock(b->mutex());
          auto const brv(b->linear_velocity() + br.cross(b->angular_velocity()));

          impulse_numerator = (brv * -(1.0 + restitution)).dot(normal);
          impulse_denominator = 1.0 / b->mass() + (br.cross(normal) * br.cross(normal)) / b->moment_of_inertia();
          impulse = impulse_numerator / impulse_denominator;

          b->linear_velocity() += normal * (impulse / b->mass());
          b->angular_velocity() += br.cross(normal * impulse) / b->moment_of_inertia();
        }
        else if(b->kinematic()) {
          std::lock_guard<std::mutex> const lock(a->mutex());
          auto const arv(a->linear_velocity() + ar.cross(a->angular_velocity()));

          impulse_numerator = (arv * -(1.0 + restitution)).dot(normal);
          impulse_denominator = 1.0 / a->mass() + (ar.cross(normal) * ar.cross(normal)) / a->moment_of_inertia();
          impulse = impulse_numerator / impulse_denominator;

          a->linear_velocity() += normal * (impulse / a->mass());
          a->angular_velocity() += ar.cross(normal * impulse) / a->moment_of_inertia();
        }
        else {
          std::lock(a->mutex(), b->mutex());
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

          a->mutex().unlock();
          b->mutex().unlock();
        }
      });
    });
  }

  void simulation::resolve_contacts() {
    parallel_for(contacts_.begin(), contacts_.end(), [&](auto const & island) {
      auto const n(island.size());
      matrix<> A(n, n);

      for(unsigned int i(0); i < n; ++i) {
        auto const & contact_i(island[i]);
        auto const & i_a(contact_i.a());
        auto const & i_b(contact_i.b());
        auto const & i_normal(contact_i.normal());

        auto const i_ar(contact_i.ap() - i_a->position());
        auto const i_br(contact_i.bp() - i_b->position());

        parallel_for_range_position(island.begin(), island.end(), [&](contact const & contact_j, std::size_t const j) {
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
        });
      }

      matrix<> B(n);
      parallel_for_range_position(island.begin(), island.end(), [&](contact const & contact, std::size_t const i) {
        auto const & a(contact.a());
        auto const & b(contact.b());
        auto const & normal(contact.normal());

        auto const ar(contact.ap() - a->position());
        auto const br(contact.bp() - b->position());

        if(!b->kinematic()) {
          auto const arv(a->linear_velocity() + ar.cross(a->angular_velocity()));
          auto const brv(b->linear_velocity() + br.cross(b->angular_velocity()));
          B(i) += 2.0 * normal.cross(b->angular_velocity()).dot(arv - brv);
        }

        B(i) += normal.dot(a->force() / a->mass() + ar.cross(a->torque() / a->moment_of_inertia()) + ar.cross(a->angular_velocity()).cross(a->angular_velocity()));
        B(i) -= normal.dot(b->force() / b->mass() + br.cross(b->torque() / b->moment_of_inertia()) + br.cross(b->angular_velocity()).cross(b->angular_velocity()));
      });

      matrix<> f(n);
      for(unsigned int i(0); i < n; ++i) {
        auto q(B(i));
        for(unsigned int j(0); j < n; ++j) {
          if(j != i) {
            q += A(i, j) * f(j);
          }
        }
        if(q >= -10e10) {
          f(i) = 0.0;
        } else {
          f(i) -= q / A(i, i);
        }
      }

      parallel_for_range_position(island.begin(), island.end(), [&](contact const & contact, std::size_t const i) {
        auto a(contact.a());
        auto b(contact.b());
        auto const & normal(contact.normal());

        auto const force(f(i));

        if(!a->kinematic()) {
          auto const ar(contact.ap() - a->position());
          std::lock_guard<std::mutex> const lock(a->mutex());
          a->force() += normal * (force / a->mass());
          a->torque() += ar.cross(normal * force) / a->moment_of_inertia();
        }
        if(!b->kinematic()) {
          auto const br(contact.bp() - b->position());
          std::lock_guard<std::mutex> const lock(b->mutex());
          b->force() -= normal * (force / b->mass());
          b->torque() -= br.cross(normal * force) / b->moment_of_inertia();
        }
      });
    });
  }

  std::tuple<vector, vector, double, double> simulation::evaluate(std::shared_ptr<object> const & initial, double const time, double const time_step, std::tuple<vector, vector, double, double> const & derivative) const {
	  return std::make_tuple(initial->linear_velocity() + std::get<1>(derivative) * time_step,  initial->force(), initial->angular_velocity() + std::get<3>(derivative) * time_step, initial->torque());
  }

  void simulation::integrate(double const time_step) {    
    parallel_for_range(objects_.begin(), objects_.end(), [&](object_t const & object) {
      if(!object->kinematic()) {
		    auto const a(evaluate(object, time_, 0.0, std::tuple<vector, vector, double, double>()));
		    auto const b(evaluate(object, time_ + time_step * 0.5, time_step * 0.5, a));
		    auto const c(evaluate(object, time_ + time_step * 0.5, time_step * 0.5, b));
		    auto const d(evaluate(object, time_ + time_step, time_step, c));

		    object->position() += (std::get<0>(a) + (std::get<0>(b) + std::get<0>(c)) * 2.0 + std::get<0>(d)) * (1.0 / 6.0) * time_step;
		    object->linear_velocity() += (std::get<1>(a) + (std::get<1>(b) + std::get<1>(c)) * 2.0 + std::get<1>(d)) * (1.0 / 6.0) * time_step;
		    object->orientation() += (std::get<2>(a) + (std::get<2>(b) + std::get<2>(c)) * 2.0 + std::get<2>(d)) * (1.0 / 6.0) * time_step;
		    object->angular_velocity() += (std::get<3>(a) + (std::get<3>(b) + std::get<3>(c)) * 2.0 + std::get<3>(d)) * (1.0 / 6.0) * time_step;

        auto const movement_threshold(0.01);
        if(object->linear_velocity().length() <= movement_threshold && std::abs(object->angular_velocity()) <= movement_threshold) {
          object->frozen(true);
          object->linear_velocity() = vector();
          object->angular_velocity() = 0.0;
        } else {
          object->frozen(false);
        }
      }
    });
  }

}
