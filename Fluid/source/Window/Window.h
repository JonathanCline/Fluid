#pragma once

#include "Utility/Pimpl.h"

#include "GLFW.h"

#include <string>

struct GLFWwindow;

namespace PROJECT_NAMESPACE
{
	namespace impl
	{
		void destroy_window(GLFWwindow*& _window);
	};

	class Window
	{
	protected:
		explicit Window(GLFWwindow* _window) :
			glfw_{}, window_{ _window }
		{};

	public:

		explicit Window(std::nullptr_t) :
			Window{ (GLFWwindow*)nullptr }
		{};

		Window(int _width, int _height, const std::string& _title);
		Window(int _width, int _height);
		Window();

	private:
		GLFWLib glfw_{};
		pimpl_ptr<GLFWwindow, impl::destroy_window> window_{ nullptr };
	};
};
