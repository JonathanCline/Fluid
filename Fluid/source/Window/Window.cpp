#include "Window.h"

#include "Utility/Singleton.h"

#include <GLFW/glfw3.h>

namespace PROJECT_NAMESPACE
{
	namespace impl
	{
		void destroy_window(GLFWwindow*& _window)
		{
			glfwDestroyWindow(_window);
			_window = nullptr;
		};
	};

	Window::Window(int _width, int _height, const std::string& _title)
	{
		this->window_ = decltype(this->window_){ glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL) };
	};
	Window::Window(int _width, int _height) :
		Window{ _width, _height, "" }
	{};
	Window::Window() :
		Window{ 800, 600 }
	{};
};
