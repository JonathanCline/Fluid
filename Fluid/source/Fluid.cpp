#include "Fluid.h"

#include "Utility/Singleton.h"
#include "Utility/Pimpl.h"

#include "Window/Window.h"

#include "ECS/ECS.h"

#include <optional>
#include <tuple>
#include <utility>
#include <concepts>

#define fluidns PROJECT_NAMESPACE

#include <GLFW/glfw3.h>

namespace fluidns
{

	struct FluidState
	{
		Window main_window{};
	};
	


	FluidState* start_fluid()
	{
		return new FluidState{};
	};
	void shutdown_fluid(FluidState*& _fluid)
	{
		delete _fluid;
		_fluid = nullptr;
	};
};


