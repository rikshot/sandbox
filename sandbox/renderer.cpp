#include <memory>

#include "renderer.hpp"
#include "simulation.hpp"

extern std::shared_ptr<sandbox::simulation> simulation;
extern std::shared_ptr<sandbox::object> object1;

namespace sandbox {

renderer::renderer(int unsigned const width, int unsigned const height) {
  window_ = glfwCreateWindow(width, height, "Sandbox", NULL, NULL);

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
  gluOrtho2D(0.0f, width, height, 0.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.375f, 0.375f, 0.0f);
}

void renderer::clear() const {
  glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::render(std::shared_ptr<object> const& object) const {
  render(object->getShape().vertices(), object->position(), object->orientation());
  /*glColor3f(1.0f, 0.0f, 0.0f);
render(object->shape().core().vertices(), object->position(), object->orientation());
  glColor3f(0.0f, 0.0f, 0.0f);
  render(object->shape().centroid() + object->position());
  glColor3f(1.0f, 1.0f, 1.0f);*/
}

void renderer::render(std::vector<vector> const& vertices, vector const& position, float const orientation) const {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, 0.375f, 0.0f);

  glTranslatef(position.x(), position.y(), 0.0f);
  glRotatef(orientation * 57.295779513082320876798154814105f, 0.0f, 0.0f, 1.0f);

  glBegin(GL_POLYGON);
  for(auto vertex : vertices) {
    glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
  };
  glEnd();
}

void renderer::render(std::vector<vector> const& vertices) const {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, 0.375f, 0.0f);

  glBegin(GL_POLYGON);
  for(auto vertex : vertices) {
    glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
  };
  glEnd();
}

void renderer::render(vector const& top_left,
                      vector const& top_right,
                      vector const& bottom_right,
                      vector const& bottom_left) const {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.375f, 0.375f, 0.0f);

  glBegin(GL_QUADS);
  glVertex2i(static_cast<int>(top_left.x()), static_cast<int>(top_left.y()));
  glVertex2i(static_cast<int>(top_right.x()), static_cast<int>(top_right.y()));
  glVertex2i(static_cast<int>(bottom_right.x()), static_cast<int>(bottom_right.y()));
  glVertex2i(static_cast<int>(bottom_left.x()), static_cast<int>(bottom_left.y()));
  glEnd();
}

void renderer::render(vector const& vertex, vector const& position) const {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.375f, 0.375f, 0.0f);

  glBegin(GL_LINES);
  glVertex2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
  glVertex2i(static_cast<int>(position.x() + vertex.x()), static_cast<int>(position.y() + vertex.y()));
  glEnd();
}

void renderer::render(vector const& vertex) const {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.375f, 0.375f, 0.0f);

  glBegin(GL_POINTS);
  glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
  glEnd();
}

void renderer::render(std::string const& text, vector const& position) const {
  /*glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0.375f, 0.375f, 0.0f);

  glRasterPos2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
  glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());*/
}

void renderer::swap_buffers() const {
  glfwSwapBuffers(window_);
}
}
