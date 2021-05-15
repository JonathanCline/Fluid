#pragma once

namespace PROJECT_NAMESPACE
{
	struct GLFWLib
	{
		GLFWLib();
		
		GLFWLib(const GLFWLib& other) = delete;
		GLFWLib& operator=(const GLFWLib& other) = delete;

		GLFWLib(GLFWLib&& other) noexcept;
		GLFWLib& operator=(GLFWLib&& other) = default;

		~GLFWLib();
	};
};