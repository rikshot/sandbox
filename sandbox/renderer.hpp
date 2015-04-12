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

  GLFWwindow * window() const {
    return window_;
  }

	void resize(int unsigned const width, int unsigned const height);

	void clear() const;

	void render(std::shared_ptr<object> const & object) const;

        void render(std::vector<Vector2f> const& vertices, Vector2f const& position, float const orientation) const;
  void render(std::vector<Vector2f> const & vertices) const;
  void render(Vector2f const & top_left, Vector2f const & top_right, Vector2f const & bottom_right, Vector2f const & bottom_left) const;
	void render(Vector2f const & vertex, Vector2f const & position) const;
	void render(Vector2f const & vertex) const;

	void render(std::string const & text, Vector2f const & position) const;

	void swap_buffers() const;

private:
	int unsigned width_;
	int unsigned height_;

  GLFWwindow * window_;
};

}
