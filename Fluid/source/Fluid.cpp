#include "Fluid.h"

#include "Window/Window.h"

#include "Utility/Memory.h"
#include "Utility/Singleton.h"
#include "Utility/Pimpl.h"

#include "ECS/FluidECS.h"

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
};



/*
	Fluid state management and major interactions
*/

namespace fluid
{
	namespace impl
	{
		extern inline std::unique_ptr<FluidState> FLUID_STATE{ nullptr };
		auto& get_fluid_state()
		{
			return FLUID_STATE;
		};

		bool has_fluid_state()
		{
			const auto& _ptr = get_fluid_state();
			return (bool)_ptr;
		};
		FluidState& fluid_state()
		{
			assert(has_fluid_state());
			return *get_fluid_state();
		};
	};
	using impl::fluid_state;


	bool start_fluid()
	{
		assert(!impl::has_fluid_state());
		auto& _fstate = impl::get_fluid_state();
		
		assert(!_fstate); // sanity checking for lack of fluid state presence
		_fstate = make_unique<FluidState>();
		
		assert(_fstate && impl::has_fluid_state()); // sanity checking for fluid state presence
		return true;
	};


	void shutdown_fluid()
	{
		auto& _fluid = impl::get_fluid_state();
		if (_fluid)
		{
			assert(impl::has_fluid_state());
			impl::get_fluid_state().reset();
			_fluid = nullptr;
		};

		// make sure synced
		assert(!_fluid && !impl::has_fluid_state());
	};
	void update()
	{
		auto& _fluid = fluid_state();

		auto& _window = _fluid.main_window.window;
		_window.clear();
		_fluid.ecs.update();
		_window.swap_buffers();

		glfwPollEvents();

	};


	FluidEntity get_main_window()
	{
		auto& _fluid = fluid_state();
		return _fluid.main_window.entity;
	};

};



/*
	General ECS interactions
*/

namespace fluid
{
	FluidEntity new_entity()
	{
		auto& _fstate = fluid_state();
		return _fstate.ecs.new_entity();
	};
	void destroy_entity(FluidEntity& _entity)
	{
		auto& _fstate = fluid_state();
		_fstate.ecs.destroy_entity(_entity);
	};




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


	void add_component(FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_type);
		assert(!_system->contains(_entity));
		_system->insert(_entity);
	};
	bool has_component( FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_type);
		return _system->contains(_entity);
	};
	void remove_component( FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_type);
		_system->erase(_entity);
	};


};



/*
	Script component interactions
*/

namespace fluid
{
	void set_script_path(FluidEntity _entity, const std::filesystem::path& _path, bool _reloadOnChange)
	{
		auto& _component = get_component<ctScript>(_entity);
		_component.source() = fluid::make_unique<Source_File>(_path);
	};

	void set_script_source(FluidEntity _entity, const std::string& _str)
	{
		auto& _component = get_component<ctScript>(_entity);
		_component.source() = fluid::make_unique<Source_Data<std::string>>(_str);
	};

	
	bool reload_script(FluidEntity _entity)
	{
		assert(has_component(_entity, ctScript));
		auto& _comp = get_component<ctScript>(_entity);
		return _comp.reload();
	};
	bool resume_script(FluidEntity _entity)
	{
		assert(has_component(_entity, ctScript));
		auto& _comp = get_component<ctScript>(_entity);
		return _comp.resume() != _comp.Error;
	};
	bool run_script(FluidEntity _entity)
	{
		assert(has_component(_entity, ctScript));
		auto& _comp = get_component<ctScript>(_entity);
		return _comp.resume() != _comp.Error;
	};


};



/*
	Element component interactions
*/

namespace fluid
{
	std::vector<FluidEntity> get_elements()
	{
		auto& _system = get_system<ctElement>();
		auto& _vec = _system->container();

		std::vector<FluidEntity> _out{};
		_out.resize(_vec.size());
		auto _it = _out.begin();
		for (auto& e : _vec)
		{
			*_it = e.first;
			++_it;
		};

		return _out;
	};

	void set_element_name( FluidEntity _entity, const std::string& _name)
	{
		auto& _comp = get_component<ctElement>(_entity);
		_comp.name = _name;
	};
	std::string get_element_name( FluidEntity _entity)
	{
		const auto& _comp = get_component<ctElement>(_entity);
		return _comp.name;
	};
};



/*
	Widget component interactions
*/

namespace fluid
{
	void set_widget_close_callback(FluidEntity _entity, WidgetCloseCallback _callback)
	{
		auto& _comp = get_component<ctWidget>(_entity);
		_comp.on_close = _callback;
	};

};

