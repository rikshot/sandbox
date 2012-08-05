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

	boost::shared_ptr<sandbox::object> const wall_top(new sandbox::object(sandbox::shape(sandbox::rectangle(width, 1.0)), wall_material));
	wall_top->position() = sandbox::vector(half_width, -1.0);
	wall_top->kinematic(true);
	simulation->objects().insert(std::make_pair("wall_top", wall_top));

	boost::shared_ptr<sandbox::object> const wall_right(new sandbox::object(sandbox::shape(sandbox::rectangle(1.0, height)), wall_material));
	wall_right->position() = sandbox::vector(width + 1.0, half_height);
	wall_right->kinematic(true);
	simulation->objects().insert(std::make_pair("wall_right", wall_right));

	boost::shared_ptr<sandbox::object> const wall_bottom(new sandbox::object(sandbox::shape(sandbox::rectangle(width, 1.0)), wall_material));
	wall_bottom->position() = sandbox::vector(half_width, height + 1.0);
	wall_bottom->kinematic(true);
	simulation->objects().insert(std::make_pair("wall_bottom", wall_bottom));

	boost::shared_ptr<sandbox::object> const wall_left(new sandbox::object(sandbox::shape(sandbox::rectangle(1.0, height)), wall_material));
	wall_left->position() = sandbox::vector(-1.0, half_height);
	wall_left->kinematic(true);
	simulation->objects().insert(std::make_pair("wall_left", wall_left));

	boost::shared_ptr<sandbox::object> const object1(new sandbox::object(sandbox::shape(sandbox::circle(50.0, 3)), sandbox::material(0.01, 0.5)));
	object1->position() = sandbox::vector(half_width, half_height);
	object1->orientation() = 45.0 * 0.01745329251994329576923690768489;
	simulation->objects().insert(std::make_pair("object1", object1));

	boost::shared_ptr<sandbox::object> const object2(new sandbox::object(sandbox::shape(sandbox::circle(25.0, 4)), sandbox::material(0.01, 0.5)));
	object2->position() = sandbox::vector(half_width, half_height + 100);
	object2->orientation() = 45.0 * 0.01745329251994329576923690768489;
	object2->kinematic(true);
	simulation->objects().insert(std::make_pair("object2", object2));
	
	/*boost::shared_ptr<sandbox::object> const object3(new sandbox::object(sandbox::shape(sandbox::circle(25.0, 4)), sandbox::material(0.000001, 0.4)));
	object3->position() = sandbox::vector(half_width + 40, half_height + 100);
	object3->orientation() = 45.0 * 0.01745329251994329576923690768489;
	object3->kinematic(true);
	simulation->objects().insert(std::make_pair("object3", object3));*/

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

		for(auto i(simulation->objects().begin()); i != simulation->objects().end(); ++i) renderer->render(i->second);

		sandbox::shape const shape1(sandbox::shape::transform(object1->shape().core(), object1->position(), object1->orientation()));
		sandbox::shape const shape2(sandbox::shape::transform(object2->shape().core(), object2->position(), object2->orientation()));
		boost::tuple<bool, sandbox::vector, double, sandbox::vector, sandbox::vector> const distance_data(shape1.distance(shape2));
		sandbox::vector const & normal(distance_data.get<1>());
		double const & distance(distance_data.get<2>());
		sandbox::vector const & point1(distance_data.get<3>());
		sandbox::vector const & point2(distance_data.get<4>());

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