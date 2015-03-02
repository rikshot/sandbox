#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <thread>
#include <mutex>

#include "object.hpp"
#include "contact.hpp"
#include "quadtree.hpp"

namespace sandbox {

  class simulation {
  public:
      typedef std::shared_ptr<object> object_t;

      simulation(double const width, double const height) : width_(width), height_(height), time_(0.0), accumulator_(0.0), quadtree_(rectangle(vector(0.0, 0.0), vector(width_, height_))) {
      }

      std::vector<object_t> const & objects() const {
        return objects_;
      }

      std::vector<object_t> & objects() {
        return objects_;
      }

      std::unordered_map<object_t, rectangle> const & bounding_boxes() const {
        return bounding_boxes_;
      }

      std::vector<std::vector<contact>> const & contacts() const {
        return contacts_;
      }

      quadtree const & getQuadtree() const {
        return quadtree_;
      }

      double time() const {
        return time_;
      }

      void step(double const delta_time, double const time_step);

    private:
      double const width_;
      double const height_;

      double time_;
      double accumulator_;

      std::vector<object_t> objects_;

      std::unordered_map<object_t, shape> world_shapes_;
      std::mutex world_shapes_mutex_;

      std::unordered_map<object_t, rectangle> bounding_boxes_;
      std::mutex bounding_boxes_mutex_;

      sandbox::quadtree quadtree_;

      std::unordered_map<object_t, std::unordered_set<object_t>> collisions_;
      std::mutex collisions_mutex_;

      std::set<std::shared_ptr<std::set<std::pair<object_t, object_t>>>> islands_;
      std::mutex islands_mutex_;

      std::vector<std::vector<contact>> contacts_;
      std::mutex contacts_mutex_;

      void update_world_shapes();
      void update_bounding_boxes();
      void update_quadtree();

      void find_collisions();
      void find_islands();
      void find_contacts();

      void resolve_collisions();
      void resolve_contacts();

      std::tuple<vector, vector, double, double> evaluate(object_t const & initial, double const time, double const time_step, std::tuple<vector, vector, double, double> const & derivative) const;
      void integrate(double const time_step);
  };

}
