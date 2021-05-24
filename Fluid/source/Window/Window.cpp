#include "Window.h"

#include "Utility/Singleton.h"

#include "ECS/Entity.h"

#include <glad/glad.h>
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


	void Window::clear(unsigned _bits)
	{
		glClear(_bits);
	};
	void Window::clear()
	{
		return this->clear(this->buffer_bits_);
	}; 


	void Window::swap_buffers()
	{
		glfwSwapBuffers(this->window_.get());
	};



	Window::Window(int _width, int _height, const std::string& _title)
	{
		this->window_ = decltype(this->window_){ glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL) };
		const auto& _window = this->window_.get();
		
		glfwMakeContextCurrent(_window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	};
	Window::Window(int _width, int _height) :
		Window{ _width, _height, "" }
	{};
	Window::Window() :
		Window{ 800, 600 }
	{};
};
