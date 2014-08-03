#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <memory>

#include "object.hpp"
#include "material.hpp"

namespace sandbox {

class renderer {
public:
	renderer(int unsigned const width, int unsigned const height);
	~renderer();

	int unsigned width() const {
		return width_;
	}
	
	int unsigned height() const {
		return height_;
	}

  GLFWwindow * const window() const {
    return window_;
  }
	
	void resize(int unsigned const width, int unsigned const height);

	void clear() const;

	void render(std::shared_ptr<object> const & object) const;
	
	void render(std::vector<vector> const & vertices, vector const & position, double const orientation) const;
  void render(std::vector<vector> const & vertices) const;
  void render(vector const & top_left, vector const & top_right, vector const & bottom_right, vector const & bottom_left) const;
	void render(vector const & vertex, vector const & position) const;
	void render(vector const & vertex) const;

	void render(std::string const & text, vector const & position) const;

	void swap_buffers() const;

private:
	int unsigned width_;
	int unsigned height_;

  GLFWwindow * window_;
};

}
