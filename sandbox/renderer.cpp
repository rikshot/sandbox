#include "renderer.hpp"
#include "simulation.hpp"

#include <boost\scoped_ptr.hpp>

extern boost::scoped_ptr<sandbox::simulation> simulation;
extern bool running;
extern bool paused;
extern bool single;

namespace sandbox {

renderer::renderer(int unsigned const width, int unsigned const height) {
	WNDCLASSEX const window_class = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		static_cast<WNDPROC>(window_procedure),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"window_class",
		0
	};

	RegisterClassEx(&window_class);

	window_ = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		"window_class",
		"Sandbox",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		0,
		0
	);

	device_context_ = GetDC(window_);
	
	PIXELFORMATDESCRIPTOR const pixel_format_descriptor = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		16,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};

	SetPixelFormat(device_context_, ChoosePixelFormat(device_context_, &pixel_format_descriptor), &pixel_format_descriptor);

	rendering_context_ = wglCreateContext(device_context_);
	wglMakeCurrent(device_context_, rendering_context_);
	
	SelectObject(device_context_, GetStockObject(SYSTEM_FONT));
	wglUseFontBitmaps(device_context_, 0, 255, 0);
	glListBase(0);

	glPointSize(5.0f);

	RECT client_rect = {0};
	GetClientRect(window_, &client_rect);
	resize(client_rect.right, client_rect.bottom);
}

renderer::~renderer() {
	wglMakeCurrent(0, 0);
	wglDeleteContext(rendering_context_);
	ReleaseDC(window_, device_context_);
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

void renderer::render(boost::shared_ptr<object> const & object) const {
	render(object->shape().vertices(), object->position(), object->orientation());
	glColor3d(1.0, 0.0, 0.0);
	render(object->shape().core(), object->position(), object->orientation());
	glColor3d(0.0, 0.0, 0.0);
	render(object->shape().centroid() + object->position());
	glColor3d(1.0, 1.0, 1.0);
}

void renderer::render(std::vector<vector> const & vertices, vector const & position, double const orientation) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glTranslated(position.x(), position.y(), 0.0);
	glRotated(orientation * 57.295779513082320876798154814105, 0.0, 0.0, 1.0);

	glBegin(GL_POLYGON);
	for(int unsigned i(0); i < vertices.size(); ++i) {
		vector const & vertex(vertices[i]);
		glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
	};
	glEnd();
}

void renderer::render(vector const & vertex, vector const & position) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_LINES);
	glVertex2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
	glVertex2i(static_cast<int>(position.x() + vertex.x()), static_cast<int>(position.y() + vertex.y()));
	glEnd();
}

void renderer::render(vector const & vertex) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);

	glBegin(GL_POINTS);
	glVertex2i(static_cast<int>(vertex.x()), static_cast<int>(vertex.y()));
	glEnd();
}

void renderer::render(std::string const & text, vector const & position) const {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.375, 0.375, 0.0);
	
	glRasterPos2i(static_cast<int>(position.x()), static_cast<int>(position.y()));
	glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());
}

void renderer::swap_buffers() const {
	SwapBuffers(device_context_);
}

LRESULT CALLBACK renderer::window_procedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_KEYDOWN:
		switch(wParam) {
		case VK_UP:
			::simulation->objects()["object1"]->linear_velocity() += sandbox::vector(0.0, -5.0);
			break;

		case VK_DOWN:
			::simulation->objects()["object1"]->linear_velocity() += sandbox::vector(0.0, 5.0);
			break;
		
		case VK_LEFT:
			::simulation->objects()["object1"]->linear_velocity() += sandbox::vector(-5.0, 0.0);
			break;
		
		case VK_RIGHT:
			::simulation->objects()["object1"]->linear_velocity() += sandbox::vector(5.0, 0.0);
			break;

		case VK_ADD:
			::simulation->objects()["object1"]->angular_velocity() += 5.0 * 0.01745329251994329576923690768489;
			break;

		case VK_SUBTRACT:
			::simulation->objects()["object1"]->angular_velocity() -= 5.0 * 0.01745329251994329576923690768489;
			break;

		case VK_RETURN:
			if(::running) {
				::running = false;
				::paused = true;
			}
			else ::running = true;
			break;

		case VK_SPACE:
			::running = false;
			::paused = true;
			::single = true;
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

}