#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <random>

#include "simulation.hpp"
#include "renderer.hpp"
#include "object.hpp"
#include "shape.hpp"
#include "misc.hpp"
#include "quadtree.hpp"

#ifndef NDEBUG
#define SANDBOX_DEBUG
#endif

#ifdef SANDBOX_DEBUG
#define SANDBOX_DRAW_CORES
#define SANDBOX_DRAW_CONTACTS
#define SANDBOX_DRAW_QUADTREES
#define SANDBOX_DRAW_ISLANDS
#define SANDBOX_DRAW_BOUNDING_BOXES
#endif

std::shared_ptr<sandbox::simulation> simulation;
std::shared_ptr<sandbox::renderer> renderer;

std::shared_ptr<sandbox::object> object1;

bool running = true;
bool paused = false;
bool single = false;

static void error_callback(int error, const char* description) {
  std::cerr << error << ": " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch(key) {
    case GLFW_KEY_UP:
      object1->linear_velocity() += sandbox::vector(0.0f, -5.0f);
      break;

    case GLFW_KEY_DOWN:
      object1->linear_velocity() += sandbox::vector(0.0f, 5.0f);
      break;

    case GLFW_KEY_LEFT:
      object1->linear_velocity() += sandbox::vector(-5.0f, 0.0f);
      break;

    case GLFW_KEY_RIGHT:
      object1->linear_velocity() += sandbox::vector(5.0f, 0.0f);
      break;

    case GLFW_KEY_KP_ADD:
      object1->angular_velocity() += 5.0f * 0.01745329251994329576923690768489f;
      break;

    case GLFW_KEY_KP_SUBTRACT:
      object1->angular_velocity() -= 5.0f * 0.01745329251994329576923690768489f;
      break;

    case GLFW_KEY_ENTER:
      if(running) {
        running = false;
        paused = true;
      } else {
        running = true;
      }
      break;

    case GLFW_KEY_SPACE:
      running = false;
      paused = true;
      single = true;
      break;
  }
}

int main() {
  std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';

  glfwSetErrorCallback(error_callback);
  glfwInit();

  simulation.reset(new sandbox::simulation(800, 600));
  renderer.reset(new sandbox::renderer(800, 600));

  glfwSetKeyCallback(renderer->window(), key_callback);

  int unsigned const width(640);
  int unsigned const height(480);

  int unsigned const half_width(width / 2);
  int unsigned const half_height(height / 2);

  sandbox::vector const offset((renderer->width() - width) / 2, (renderer->height() - height) / 2);

  sandbox::material const wall_material(1.0f, 0.0f, sandbox::color<>(1.0f, 1.0f, 1.0f, 1.0f));

  std::shared_ptr<sandbox::object> const wall_top(
      new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0f, height).vertices()), wall_material));
  wall_top->position() = sandbox::vector(half_width, 0.0f - half_height) + offset;
  wall_top->kinematic(true);
  simulation->objects().push_back(wall_top);

  std::shared_ptr<sandbox::object> const wall_right(
      new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0f).vertices()), wall_material));
  wall_right->position() = sandbox::vector(width + half_width, half_height) + offset;
  wall_right->kinematic(true);
  simulation->objects().push_back(wall_right);

  std::shared_ptr<sandbox::object> const wall_bottom(
      new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0f, height).vertices()), wall_material));
  wall_bottom->position() = sandbox::vector(half_width, height + half_height) + offset;
  wall_bottom->kinematic(true);
  simulation->objects().push_back(wall_bottom);

  std::shared_ptr<sandbox::object> const wall_left(
      new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0f).vertices()), wall_material));
  wall_left->position() = sandbox::vector(0.0f - half_width, half_height) + offset;
  wall_left->kinematic(true);
  simulation->objects().push_back(wall_left);

  object1.reset(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()),
                                    sandbox::material(1.0f, 0.0f, sandbox::color<>(1.0f, 1.0f, 1.0f, 1.0f))));
  object1->position() = sandbox::vector(half_width, half_height + 160) + offset;
  simulation->objects().push_back(object1);

  std::shared_ptr<sandbox::object> const object2(
      new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()),
                          sandbox::material(1.0f, 0.0f, sandbox::color<>(1.0f, 1.0f, 1.0f, 1.0f))));
  object2->position() = sandbox::vector(half_width + 40, half_height + 200) + offset;
  simulation->objects().push_back(object2);

  for(unsigned y(0); y < 10; ++y) {
    for(unsigned x(0); x < 3; ++x) {
      std::shared_ptr<sandbox::object> const object(
          new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()),
                              sandbox::material(1.0f, 0.0f, sandbox::color<>(1.0f, 1.0f, 1.0f, 1.0f))));
      object->position() = sandbox::vector(half_width - (4 / 2 * 25) + ((x + 1) * 25), (y + 1) * 25) + offset;
      simulation->objects().push_back(object);
    }
  }

  float const time_step(0.001f);
  float time(glfwGetTime());

  std::stringstream fps;
  unsigned int frames_per_second(0);
  float frame_counter(1.0f);

  while(!glfwWindowShouldClose(renderer->window())) {
    if(single) {
      simulation->step(0.01f, time_step);
      single = false;
    } else if(running) {
      float const new_time(glfwGetTime());
      float const delta_time(new_time - time);
      frame_counter += delta_time;
      if(frame_counter >= 1.0f) {
        fps.str(std::string());
        fps << "FPS: " << frames_per_second;

        frames_per_second = 0;
        frame_counter = 0.0f;
      }
      simulation->step(paused ? 0 : delta_time, time_step);
      if(paused)
        paused = false;
      time = new_time;
    }

    renderer->clear();

    for(auto object : simulation->objects()) {
      if(object->frozen()) {
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      } else {
        auto const& color(object->getMaterial().getColor());
        glColor4f(color.red(), color.green(), color.blue(), color.alpha());
      }
      renderer->render(object);

#ifdef SANDBOX_DRAW_CORES
      glColor4f(0.5, 0.5, 0.5, 1.0f);
      renderer->render(object->getShape().core().vertices(), object->position(), object->orientation());
#endif

#ifdef SANDBOX_DRAW_BOUNDING_BOXES
      if(!simulation->bounding_boxes().empty()) {
        glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderer->render(simulation->bounding_boxes().at(object).vertices());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      }
#endif
    }

#ifdef SANDBOX_DRAW_QUADTREES
    simulation->getQuadtree().visit([](sandbox::quadtree::node const* const node) {
      auto const& rectangle = node->getRectangle();
      glColor3f(0.0f, 0.0f, 1.0f);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      renderer->render(rectangle.vertices(), sandbox::vector(), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glColor3f(1.0f, 1.0f, 1.0f);
    });
#endif

#if defined(SANDBOX_DRAW_ISLANDS) || defined(SANDBOX_DRAW_CONTACTS)
    glColor4f(0.0f, 0.75, 0.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(auto const& island : simulation->contacts()) {
      for(auto const& contact : island) {
#ifdef SANDBOX_DRAW_ISLANDS
        renderer->render(contact.b()->position() - contact.a()->position(), contact.a()->position());
        renderer->render(contact.a()->position());
        renderer->render(contact.b()->position());
#endif
#ifdef SANDBOX_DRAW_CONTACTS
        renderer->render(contact.ap());
        renderer->render(contact.bp());
#endif
      }
    }
#endif

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    renderer->render(fps.str(), sandbox::vector(10.0f, 20.0f));

    std::stringstream time;
    time << "Time: " << simulation->time();
    renderer->render(time.str(), sandbox::vector(10.0f, 40.0f));

    std::stringstream position;
    position << "Position: (" << object1->position().x() << ", " << object1->position().y() << ")";
    renderer->render(position.str(), sandbox::vector(10.0f, 60.0f));

    std::stringstream linear_velocity;
    linear_velocity << "Linear Velocity: (" << object1->linear_velocity().x() << ", " << object1->linear_velocity().y()
                    << ")";
    renderer->render(linear_velocity.str(), sandbox::vector(10.0f, 80.0f));

    std::stringstream orientation;
    orientation << "Orientation: " << object1->orientation();
    renderer->render(orientation.str(), sandbox::vector(10.0f, 100.0f));

    std::stringstream angular_velocity;
    angular_velocity << "Angular Velocity: " << object1->angular_velocity();
    renderer->render(angular_velocity.str(), sandbox::vector(10.0f, 120.0f));

    std::stringstream force;
    force << "Force: " << object1->force().x() << ", " << object1->force().y() << ")";
    renderer->render(force.str(), sandbox::vector(10.0f, 140.0f));

    renderer->swap_buffers();

    glfwPollEvents();

    ++frames_per_second;
  }

  renderer.reset();

  glfwTerminate();
}
