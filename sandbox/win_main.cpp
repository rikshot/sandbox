#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>

#include <boost\shared_ptr.hpp>
#include <boost\scoped_ptr.hpp>

#include "simulation.hpp"
#include "renderer.hpp"
#include "object.hpp"
#include "shape.hpp"
#include "misc.hpp"

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
	setvbuf( stderr, NULL, _IONBF, 0 );

	std::ios::sync_with_stdio();

	simulation.reset(new sandbox::simulation());
	renderer.reset(new sandbox::renderer(640, 480));
	
	int unsigned const width(renderer->width());
	int unsigned const height(renderer->height());

	int unsigned const half_width(width / 2);
	int unsigned const half_height(height / 2);

	sandbox::material const wall_material(1.0, 0.1);

	boost::shared_ptr<sandbox::object> const wall_top(new sandbox::object(sandbox::shape(sandbox::rectangle(width, 10.0)), wall_material));
	wall_top->position() = sandbox::vector(half_width, 0.0);
	wall_top->kinematic(true);
	simulation->objects().push_back(wall_top);

	boost::shared_ptr<sandbox::object> const wall_right(new sandbox::object(sandbox::shape(sandbox::rectangle(10.0, height)), wall_material));
	wall_right->position() = sandbox::vector(width, half_height);
	wall_right->kinematic(true);
	simulation->objects().push_back(wall_right);

	boost::shared_ptr<sandbox::object> const wall_bottom(new sandbox::object(sandbox::shape(sandbox::rectangle(width, 10.0)), wall_material));
	wall_bottom->position() = sandbox::vector(half_width, height);
	wall_bottom->kinematic(true);
  simulation->objects().push_back(wall_bottom);

	boost::shared_ptr<sandbox::object> const wall_left(new sandbox::object(sandbox::shape(sandbox::rectangle(10.0, height)), wall_material));
	wall_left->position() = sandbox::vector(0.0, half_height);
	wall_left->kinematic(true);
	simulation->objects().push_back(wall_left);

	object1.reset(new sandbox::object(sandbox::shape(sandbox::circle(50.0, 3)), sandbox::material(0.01, 0.5)));
	object1->position() = sandbox::vector(half_width, half_height);
	object1->orientation() = 45.0 * 0.01745329251994329576923690768489;
	simulation->objects().push_back(object1);

  for(unsigned i(0); i < 11; ++i) {
    boost::shared_ptr<sandbox::object> const object(new sandbox::object(sandbox::shape(sandbox::circle(25.0, 6)), sandbox::material(0.0001, 0.4)));
    object->orientation() = i * 10;
	  object->position() = sandbox::vector(50 * (i + 1), 50);
    object->linear_velocity() = sandbox::vector(25.0, 0.0);
	  simulation->objects().push_back(object);
  }

	LARGE_INTEGER raw_frequency = {0};
	QueryPerformanceFrequency(&raw_frequency);
	double const frequency(static_cast<double>(raw_frequency.QuadPart));
	
	LARGE_INTEGER raw_time = {0};
	QueryPerformanceCounter(&raw_time);
	double time(static_cast<double>(raw_time.QuadPart) / frequency);

	MSG message = {0};
	for(;;) { 
		while(PeekMessage(&message, 0, 0, 0, PM_NOREMOVE)) { 
			if(GetMessage(&message, 0, 0, 0) != 0) { 
				TranslateMessage(&message); 
				DispatchMessage(&message); 
			} 
			else return static_cast<int>(message.wParam);
		}
		
		renderer->clear();
		
		if(single) {
			simulation->step(0.01, 0.01);
			single = false;
		}
		else if(running) {
			QueryPerformanceCounter(&raw_time);
			double const new_time(static_cast<double>(raw_time.QuadPart) / frequency);
			simulation->step(paused ? 0 : new_time - time, 0.01);
			if(paused) paused = false;
			time = new_time;
		}

    for(auto object : simulation->objects()) renderer->render(object);

		glColor3d(1.0, 1.0, 0.0);
		for(auto i(simulation->contacts().begin()); i != simulation->contacts().end(); ++i) {
			renderer->render(i->ap());
			renderer->render(i->bp());
		}
		glColor3d(1.0, 1.0, 1.0);

		std::stringstream time;
		time << "Time: " << simulation->time();
		renderer->render(time.str(), sandbox::vector(10.0, 20.0));

		std::stringstream position;
		position << "Position: (" << object1->position().x() << ", " << object1->position().y() << ")";
		renderer->render(position.str(), sandbox::vector(10.0, 40.0));

		std::stringstream linear_velocity;
		linear_velocity << "Linear Velocity: (" << object1->linear_velocity().x() << ", " << object1->linear_velocity().y() << ")";
		renderer->render(linear_velocity.str(), sandbox::vector(10.0, 60.0));

		std::stringstream orientation;
		orientation << "Orientation: " << object1->orientation();
		renderer->render(orientation.str(), sandbox::vector(10.0, 80.0));

		std::stringstream angular_velocity;
		angular_velocity << "Angular Velocity: " << object1->angular_velocity();
		renderer->render(angular_velocity.str(), sandbox::vector(10.0, 100.0));

		renderer->swap_buffers();
	}
}