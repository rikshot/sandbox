#pragma once

#include <Windows.h>
#include <WindowsX.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#include <boost/shared_ptr.hpp>

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
	
	void resize(int unsigned const width, int unsigned const height);

	void clear() const;

	void render(boost::shared_ptr<object> const & object) const;
	
	void render(std::vector<vector> const & vertices, vector const & position, double const orientation) const;
	void render(vector const & vertex, vector const & position) const;
	void render(vector const & vertex) const;

	void render(std::string const & text, vector const & position) const;

	void swap_buffers() const;

private:
	int unsigned width_;
	int unsigned height_;

	HWND window_;
	HDC device_context_;
	HGLRC rendering_context_;

	static LRESULT CALLBACK window_procedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

}