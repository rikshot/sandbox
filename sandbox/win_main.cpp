#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <random>

#include <boost\shared_ptr.hpp>
#include <boost\scoped_ptr.hpp>

#include "simulation.hpp"
#include "renderer.hpp"
#include "object.hpp"
#include "shape.hpp"
#include "misc.hpp"
#include "quadtree.hpp"

boost::scoped_ptr<sandbox::simulation> simulation;
boost::scoped_ptr<sandbox::renderer> renderer;

boost::shared_ptr<sandbox::object> object1;

bool running = true;
bool paused = false;
bool single = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  AllocConsole();

  *stdout = *_fdopen(_open_osfhandle(reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT), "w");
  setvbuf(stdout, 0, _IONBF, 0);

  *stdin = *_fdopen(_open_osfhandle(reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE)), _O_TEXT), "r");
  setvbuf(stdin, 0, _IONBF, 0);

  *stderr = *_fdopen(_open_osfhandle(reinterpret_cast<intptr_t>(GetStdHandle(STD_ERROR_HANDLE)), _O_TEXT), "w");
  setvbuf(stderr, NULL, _IONBF, 0);

  std::ios::sync_with_stdio();

  simulation.reset(new sandbox::simulation(1024, 768));
  renderer.reset(new sandbox::renderer(1024, 768));

  int unsigned const width(640);
  int unsigned const height(480);

  int unsigned const half_width(width / 2);
  int unsigned const half_height(height / 2);

  sandbox::vector const offset((renderer->width() - width) / 2, (renderer->height() - height) / 2);

  sandbox::material const wall_material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0));

  boost::shared_ptr<sandbox::object> const wall_top(new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0, height).vertices()), wall_material));
  wall_top->position() = sandbox::vector(half_width, 0.0 - half_height) + offset;
  wall_top->kinematic(true);
  simulation->objects().push_back(wall_top);

  boost::shared_ptr<sandbox::object> const wall_right(new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0).vertices()), wall_material));
  wall_right->position() = sandbox::vector(width + half_width, half_height) + offset;
  wall_right->kinematic(true);
  simulation->objects().push_back(wall_right);

  boost::shared_ptr<sandbox::object> const wall_bottom(new sandbox::object(sandbox::shape(sandbox::rectangle(width * 2.0, height).vertices()), wall_material));
  wall_bottom->position() = sandbox::vector(half_width, height + half_height) + offset;
  wall_bottom->kinematic(true);
  simulation->objects().push_back(wall_bottom);

  boost::shared_ptr<sandbox::object> const wall_left(new sandbox::object(sandbox::shape(sandbox::rectangle(width, height * 2.0).vertices()), wall_material));
  wall_left->position() = sandbox::vector(0.0 - half_width, half_height) + offset;
  wall_left->kinematic(true);
  simulation->objects().push_back(wall_left);

  object1.reset(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
  object1->position() = sandbox::vector(half_width, half_height + 200) + offset;
  simulation->objects().push_back(object1);

  for(unsigned y(0); y < 10; ++y) {
    for(unsigned x(0); x < 3; ++x) {
      boost::shared_ptr<sandbox::object> const object(new sandbox::object(sandbox::shape(sandbox::rectangle(20, 20).vertices()), sandbox::material(1.0, 0.0, sandbox::color<>(1.0, 1.0, 1.0, 1.0))));
      object->position() = sandbox::vector(half_width - (4 / 2 * 25) + ((x + 1) * 25), (y + 1) * 25) + offset;
      simulation->objects().push_back(object);
    }
  }

  LARGE_INTEGER raw_frequency = { 0 };
  QueryPerformanceFrequency(&raw_frequency);
  double const frequency(static_cast<double>(raw_frequency.QuadPart));

  LARGE_INTEGER raw_time = { 0 };
  QueryPerformanceCounter(&raw_time);
  double time(static_cast<double>(raw_time.QuadPart) / frequency);

  std::stringstream fps;
  unsigned int frames_per_second(0);
  double frame_counter(1.0);

  MSG message = { 0 };
  for (;;) {
    while (PeekMessage(&message, 0, 0, 0, PM_NOREMOVE)) {
      if (GetMessage(&message, 0, 0, 0) != 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
      else return static_cast<int>(message.wParam);
    }

    if (single) {
      simulation->step(0.01, 0.001);
      single = false;
    }
    else if (running) {
      QueryPerformanceCounter(&raw_time);
      double const new_time(static_cast<double>(raw_time.QuadPart) / frequency);
      double const delta_time(new_time - time);
      frame_counter += delta_time;
      if(frame_counter >= 1.0) {
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

      auto const & color(object->material().color());
      glColor4d(color.red(), color.green(), color.blue(), color.alpha());
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
    for(auto const & contact : simulation->find_contacts(collisions)) {
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
    ++frames_per_second;
  }
}