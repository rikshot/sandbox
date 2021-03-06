#pragma once

#include <unordered_set>
#include <memory>

#include "rectangle.hpp"
#include "object.hpp"

namespace sandbox {

  class quadtree {
  public:
    typedef std::unordered_set<std::shared_ptr<object>> set_t;

    class node {
    public:
      node(sandbox::rectangle const rectangle) : rectangle_(rectangle), nw_(nullptr), ne_(nullptr), se_(nullptr), sw_(nullptr) {}
      ~node() { 
        delete nw_;
        delete ne_;
        delete se_;
        delete sw_; 
      };

      rectangle const & getRectangle() const {
        return rectangle_;
      }

      bool insert(std::pair<std::shared_ptr<object>, sandbox::rectangle const> const & object_with_bounding_box);
      void find(sandbox::rectangle const & rectangle, set_t & objects) const;

      void visit(std::function<void (node const * const)> const & callback) const;

    private:
      sandbox::rectangle const rectangle_;
      std::vector<std::pair<std::shared_ptr<object>, sandbox::rectangle const>> objects_;
      node * nw_, * ne_, * se_, * sw_;

      void subdivide();
    };

    quadtree(rectangle const & rectangle) : rectangle_(rectangle), root_(new node(rectangle)) {}
    ~quadtree() { delete root_; }

    bool insert(std::pair<std::shared_ptr<object>, rectangle const> const & object_with_bounding_box);

    set_t find(rectangle const & rectangle) const;
    void visit(std::function<void (node const * const)> const & callback) const;

    void clear() {
      delete root_;
      root_ = nullptr;
    }
  
  private:
    rectangle const rectangle_;
    node * root_;
  };

}
