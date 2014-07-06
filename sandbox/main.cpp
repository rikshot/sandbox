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

std::shared_ptr<sandbox::simulation> simulation;
std::shared_ptr<sandbox::renderer> renderer;

std::shared_ptr<sandbox::object> object1;

bool running = true;
bool paused = false;
bool single = false;

static void error_callback(int error, const char * description) {
  std::cerr << description << std::endl;
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
  switch(key) {
    case GLFW_KEY_UP:
      object1->linear_velocity() += sandbox::vector(0.0, -5.0);
      break;

    case GLFW_KEY_DOWN:
      object1->linear_velocity() += sandbox::vector(0.0, 5.0);
      break;

    case GLFW_KEY_LEFT:
      object1->linear_velocity() += sandbox::vector(-5.0, 0.0);
      break;

    case GLFW_KEY_RIGHT:
      object1->linear_velocity() += sandbox::vector(5.0, 0.0);
      break;

    case GLFW_KEY_KP_ADD:
      object1->angular_velocity() += 5.0 * 0.01745329251994329576923690768489;
      break;

    case GLFW_KEY_KP_SUBTRACT:
      object1->angular_velocity() -= 5.0 * 0.01745329251994329576923690768489;
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

  sandbox::material const wall_material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0));

  std::shared_ptr<sandbox::object> const wall_top(new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0, height).vertices()), wall_material));
  wall_top->position() = sandbox::vector(half_width, 0.0 - half_height) + offset;
  wall_top->kinematic(true);
  simulation->objects().push_back(wall_top);

  std::shared_ptr<sandbox::object> const wall_right(new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0).vertices()), wall_material));
  wall_right->position() = sandbox::vector(width + half_width, half_height) + offset;
  wall_right->kinematic(true);
  simulation->objects().push_back(wall_right);

  std::shared_ptr<sandbox::object> const wall_bottom(new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0, height).vertices()), wall_material));
  wall_bottom->position() = sandbox::vector(half_width, height + half_height) + offset;
  wall_bottom->kinematic(true);
  simulation->objects().push_back(wall_bottom);

  std::shared_ptr<sandbox::object> const wall_left(new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0).vertices()), wall_material));
  wall_left->position() = sandbox::vector(0.0 - half_width, half_height) + offset;
  wall_left->kinematic(true);
  simulation->objects().push_back(wall_left);

  object1.reset(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
  object1->position() = sandbox::vector(half_width, half_height + 200) + offset;
  simulation->objects().push_back(object1);

  for (unsigned y(0); y < 10; ++y) {
    for (unsigned x(0); x < 3; ++x) {
      std::shared_ptr<sandbox::object> const object(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
      object->position() = sandbox::vector(half_width - (4 / 2 * 25) + ((x + 1) * 25), (y + 1) * 25) + offset;
      simulation->objects().push_back(object);
    }
  }

  double time(glfwGetTime());

  std::stringstream fps;
  unsigned int frames_per_second(0);
  double frame_counter(1.0);

  while(!glfwWindowShouldClose(renderer->window())) {
    if (single) {
      simulation->step(0.01, 0.001);
      single = false;
    } else if (running) {
      double const new_time(glfwGetTime());
      double const delta_time(new_time - time);
      frame_counter += delta_time;
      if (frame_counter >= 1.0) {
        fps.str(std::string());
        fps << "FPS: " << frames_per_second;

        frames_per_second = 0;
        frame_counter = 0.0;
      }
      simulation->step(paused ? 0 : delta_time, 0.001);
      if (paused) paused = false;
      time = new_time;
    }

    renderer->clear();

    //sandbox::quadtree qt(sandbox::rectangle(sandbox::vector(0.0, 0.0) + offset, sandbox::vector(width, height) + offset));

    for (auto object : simulation->objects()) {
      //auto const bounding_box(object->shape().transform(object->position(), object->orientation()).bounding_box());
      //qt.insert(std::make_pair(object, bounding_box));

      if (object->frozen()) {
        glColor4d(0.0, 0.0, 1.0, 1.0);
      } else {
        auto const & color(object->material().color());
        glColor4d(color.red(), color.green(), color.blue(), color.alpha());
      }
      renderer->render(object);

      glColor4d(0.5, 0.5, 0.5, 1.0);
      renderer->render(object->shape().core().vertices(), object->position(), object->orientation());

      /*glColor3d(1.0, 0.0, 1.0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      renderer->render(bounding_box.vertices(), sandbox::vector(), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glColor3d(1.0, 1.0, 1.0);*/
    }

    /*auto const bounding_box(object1->shape().transform(object1->position(), object1->orientation()).bounding_box());
    auto coll(qt.find(bounding_box));
    coll.erase(object1);

    qt.visit([](sandbox::quadtree::node const * const node) {
    auto const & rectangle = node->rectangle();
    glColor3d(0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderer->render(rectangle.vertices(), sandbox::vector(), 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3d(1.0, 1.0, 1.0);
    });

    for(auto object : coll) {
    auto const bounding_box(object->shape().transform(object->position(), object->orientation()).bounding_box());
    glColor3d(1.0, 1.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderer->render(bounding_box.vertices(), sandbox::vector(), 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3d(1.0, 1.0, 1.0);
    }*/

    glColor4d(0.0, 1.0, 0.0, 1.0);
    auto const collisions(simulation->find_collisions());
    for (auto const & contact : simulation->find_contacts(collisions)) {
      renderer->render(contact.ap());
      renderer->render(contact.bp());
    }

    glColor4d(1.0, 1.0, 1.0, 1.0);

    renderer->render(fps.str(), sandbox::vector(10.0, 20.0));

    std::stringstream time;
    time << "Time: " << simulation->time();
    renderer->render(time.str(), sandbox::vector(10.0, 40.0));

    std::stringstream position;
    position << "Position: (" << object1->position().x() << ", " << object1->position().y() << ")";
    renderer->render(position.str(), sandbox::vector(10.0, 60.0));

    std::stringstream linear_velocity;
    linear_velocity << "Linear Velocity: (" << object1->linear_velocity().x() << ", " << object1->linear_velocity().y() << ")";
    renderer->render(linear_velocity.str(), sandbox::vector(10.0, 80.0));

    std::stringstream orientation;
    orientation << "Orientation: " << object1->orientation();
    renderer->render(orientation.str(), sandbox::vector(10.0, 100.0));

    std::stringstream angular_velocity;
    angular_velocity << "Angular Velocity: " << object1->angular_velocity();
    renderer->render(angular_velocity.str(), sandbox::vector(10.0, 120.0));

    std::stringstream force;
    force << "Force: " << object1->force().x() << ", " << object1->force().y() << ")";
    renderer->render(force.str(), sandbox::vector(10.0, 140.0));

    renderer->swap_buffers();
    glfwPollEvents();
    ++frames_per_second;
  }

  renderer.reset();

  glfwTerminate();
}