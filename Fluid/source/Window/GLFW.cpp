#include "GLFW.h"

#include "Utility/Guard.h"
#include "Utility/Singleton.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <cassert>

namespace PROJECT_NAMESPACE
{
	size_t& glfw_rcount() { return get_singleton<size_t, struct GLFWLibRefCount>(); }

	GLFWLib::GLFWLib()
	{
		auto& _rc = glfw_rcount();
		if (_rc == 0)
		{
			const auto _result = glfwInit();
			if (_result != GLFW_TRUE)
			{
				std::terminate();
			};
		};
		++_rc;
	};
	GLFWLib::GLFWLib(GLFWLib&& other) noexcept :
		GLFWLib{}
	{};

	GLFWLib::~GLFWLib()
	{
		auto& _rc = glfw_rcount();
		--_rc;
		if (_rc == 0)
		{
			glfwTerminate();
		};
	};
};
