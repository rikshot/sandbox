#include <iostream>

#include "quadtree.hpp"

namespace sandbox {

  bool quadtree::node::insert(std::pair<std::shared_ptr<object>, sandbox::rectangle const> const & object_with_bounding_box) {
    auto const & object(object_with_bounding_box.first);
    auto const & bounding_box(object_with_bounding_box.second);
    if(bounding_box.overlaps(rectangle_)) {
      if(objects_.size() == 4) {
        subdivide();
        nw_->insert(object_with_bounding_box);
        ne_->insert(object_with_bounding_box);
        se_->insert(object_with_bounding_box);
        sw_->insert(object_with_bounding_box);
      } else {
        objects_.push_back(object_with_bounding_box);
      }

      return true;
    }
    return false;
  }

  void quadtree::node::find(sandbox::rectangle const & rectangle, std::set<std::shared_ptr<object>> & objects) const {
    if(rectangle.overlaps(rectangle_)) {
      for(auto object_with_bounding_box : objects_) {
        auto const & object(object_with_bounding_box.first);
        auto const & bounding_box(object_with_bounding_box.second);
        if(rectangle.overlaps(bounding_box)) {
          objects.emplace(object);
        }
      }
      if(nw_) nw_->find(rectangle, objects);
      if (ne_) ne_->find(rectangle, objects);
      if (se_) se_->find(rectangle, objects);
      if (sw_) sw_->find(rectangle, objects);
    }
  }
  
  void quadtree::node::visit(std::function<void (node const * const)> const & callback) const {
    callback(this);
    if(nw_) nw_->visit(callback);
    if(ne_) ne_->visit(callback);
    if(se_) se_->visit(callback);
    if(sw_) sw_->visit(callback);
  }

  void quadtree::node::subdivide() {
    double const half_width((rectangle_.bottom_right().x() - rectangle_.top_left().x()) / 2);
    double const half_height((rectangle_.bottom_right().y() - rectangle_.top_left().y()) / 2);
    if(!nw_) nw_ = new node(sandbox::rectangle(rectangle_.top_left(), vector(rectangle_.top_left().x() + half_width, rectangle_.top_left().y() + half_height)));
    if(!ne_) ne_ = new node(sandbox::rectangle(vector(rectangle_.top_left().x() + half_width, rectangle_.top_left().y()), vector(rectangle_.bottom_right().x(), rectangle_.top_left().y() + half_height))); 
    if(!se_) se_ = new node(sandbox::rectangle(vector(rectangle_.top_left().x() + half_width, rectangle_.top_left().y() + half_height), rectangle_.bottom_right()));
    if(!sw_) sw_ = new node(sandbox::rectangle(vector(rectangle_.top_left().x(), rectangle_.top_left().y() + half_height), vector(rectangle_.top_left().x() + half_width, rectangle_.bottom_right().y())));
  }

  bool quadtree::insert(std::pair<std::shared_ptr<object>, rectangle const> const & object_with_bounding_box) {
    if(!root_) root_ = new node(rectangle_);
    return root_->insert(object_with_bounding_box);
  }

  std::set<std::shared_ptr<object>> quadtree::find(rectangle const & rectangle) const {
    std::set<std::shared_ptr<object>> objects;
    root_->find(rectangle, objects);
    return objects;
  }

  void quadtree::visit(std::function<void (node const * const)> const & callback) const {
    root_->visit(callback);
  }

}