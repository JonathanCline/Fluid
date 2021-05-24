#include "Fluid.h"
#include "FluidState.h"

/*
	Fluid state management and major interactions
*/

namespace fluid
{
	
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
	void close_widget(FluidEntity _entity)
	{
		auto& _csys = get_system<ctWidget>();
		_csys->close(_entity);
	};

};

