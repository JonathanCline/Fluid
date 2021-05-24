#pragma once

#include "Fluid.h"

#include "ECS/FluidECS.h"

#include "Window/Window.h"

#include "Utility/Memory.h"
#include "Utility/Singleton.h"
#include "Utility/Pimpl.h"

#include "LuaAPI.h"

#include <GLFW/glfw3.h>

#include <optional>
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>
#include <array>

namespace fluid
{
	struct FluidState
	{
		EntityComponentSystem ecs{};
		FluidECS csystems{};

		struct MainWindow
		{
			Window window{};
			FluidEntity entity;
		};

		MainWindow main_window;

		FluidState()
		{
			this->csystems.add_to_ecs(this->ecs);

			auto& _windowEntity = this->main_window.entity;
			_windowEntity = this->ecs.new_entity();
			this->csystems.get<Widget>()->insert(_windowEntity);
			this->csystems.get<Element>()->insert(_windowEntity);
			this->csystems.get<Element>()->at(_windowEntity).name = "MainWindow";

			// Set glfw user pointer to point to this
			auto& _window = this->main_window.window.get();
			glfwSetWindowUserPointer(_window, this);

			// Set widget close callback
			glfwSetWindowCloseCallback(_window, [](GLFWwindow* _window)
				{
					auto& _handle = *static_cast<FluidState*>(glfwGetWindowUserPointer(_window));
					auto& _csys = *_handle.csystems.get<Widget>();
					_csys.close(_handle.main_window.entity);
				});

		};

		FluidState(const FluidState&) = delete;
		FluidState(FluidState&&) = delete;
	};

	namespace impl
	{
		extern inline std::unique_ptr<FluidState> FLUID_STATE{ nullptr };
		static auto& get_fluid_state()
		{
			return FLUID_STATE;
		};

		static bool has_fluid_state()
		{
			const auto& _ptr = get_fluid_state();
			return (bool)_ptr;
		};
		static FluidState& fluid_state()
		{
			assert(has_fluid_state());
			return *get_fluid_state();
		};
	};
	using impl::fluid_state;




	template <ComponentType Type>
	static auto& get_system()
	{
		auto& _fstate = fluid_state();
		return _fstate.csystems.get<component_type_t<Type>>();
	};
	template <ComponentType Type>
	static auto& get_component(FluidEntity _entity)
	{
		auto& _system = get_system<Type>();
		assert(_system->contains(_entity));
		return _system->at(_entity);
	};


	static inline auto& get_system(ComponentType _type)
	{
		auto& _fstate = fluid_state();
		return *(_fstate.ecs.begin() + (size_t)_type);
	};


};

