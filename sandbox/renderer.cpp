#include <memory>

#include "renderer.hpp"
#include "simulation.hpp"

extern std::shared_ptr<sandbox::simulation> simulation;
extern std::shared_ptr<sandbox::object> object1;

namespace sandbox {

renderer::renderer(int unsigned const width, int unsigned const height) {
  window_ = glfwCreateWindow(
    width,
    height,
    "Sandbox",
    NULL,
    NULL
  );

  glfwMakeContextCurrent(window_);

  glPointSize(5.0f);

  int client_width, client_height;
  glfwGetFramebufferSize(window_, &client_width, &client_height);
  resize(client_width, client_height);
}

renderer::~renderer() {
	glfwDestroyWindow(window_);
}

void renderer::resize(int unsigned const width, int unsigned const height) {
	width_ = width;
	height_ = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, height, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);
}

void renderer::clear() const {
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::render(std::shared_ptr<object> const & object) const {
	render(object->getShape().vertices(), object->position(), object->orientation());
	/*glColor3d(1.0, 0.0, 0.0);
  render(object->shape().core().vertices(), object->position(), object->orientation());
	glColor3d(0.0, 0.0, 0.0);
	render(object->shape().centroid() + object->position());
	glColor3d(1.0, 1.0, 1.0);*/
}

void renderer::render(std::vector<Vector2f> const& vertices, Vector2f const& position, float const orientation) const {
        glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glTranslated(position.x(), position.y(), 0.0);
	glRotated(orientation * 57.295779513082320876798154814105, 0.0, 0.0, 1.0);

	glBegin(GL_POLYGON);
	for(auto vertex : vertices) {
		glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
	};
	glEnd();
}

void renderer::render(std::vector<Vector2f> const & vertices) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_POLYGON);
	for(auto vertex : vertices) {
		glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
	};
	glEnd();
}

void renderer::render(Vector2f const & top_left, Vector2f const & top_right, Vector2f const & bottom_right, Vector2f const & bottom_left) const {
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_QUADS);
	  glVertex2i(static_cast<int>(top_left.x()), static_cast<int>(top_left.y()));
	  glVertex2i(static_cast<int>(top_right.x()), static_cast<int>(top_right.y()));
    glVertex2i(static_cast<int>(bottom_right.x()), static_cast<int>(bottom_right.y()));
    glVertex2i(static_cast<int>(bottom_left.x()), static_cast<int>(bottom_left.y()));
	glEnd();
}

void renderer::render(Vector2f const & vertex, Vector2f const & position) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_LINES);
	  glVertex2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
	  glVertex2i(static_cast<int>(position.x() + vertex.x()), static_cast<int>(position.y() + vertex.y()));
	glEnd();
}

void renderer::render(Vector2f const & vertex) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_POINTS);
	  glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
	glEnd();
}

void renderer::render(std::string const & text, Vector2f const & position) const {
	/*glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glRasterPos2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
	glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());*/
}

void renderer::swap_buffers() const {
	glfwSwapBuffers(window_);
}

}
