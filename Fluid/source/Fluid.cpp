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

		Window main_window{};

		FluidState()
		{
			this->csystems.add_to_ecs(this->ecs);
		};
	};
};

namespace fluid::impl
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
	
	FluidState* start_fluid()
	{
		assert(!impl::has_fluid_state());
		auto& _fstate = impl::get_fluid_state();
		
		assert(!_fstate); // sanity checking for lack of fluid state presence
		_fstate = make_unique<FluidState>();
		
		assert(_fstate && impl::has_fluid_state()); // sanity checking for fluid state presence
		return _fstate.get();
	};
	void shutdown_fluid(FluidState*& _fluid)
	{
		assert(_fluid == impl::get_fluid_state().get());
		if (_fluid)
		{
			assert(impl::has_fluid_state());
			impl::get_fluid_state().reset();
			_fluid = nullptr;
		};

		// make sure synced
		assert(!_fluid && !impl::has_fluid_state());
	};
	void update(FluidState& _fluid)
	{
		_fluid.ecs.update();
	};

	FluidState& main_fluid_state()
	{
		assert(impl::has_fluid_state());
		return impl::fluid_state();
	};
};
namespace fluid
{
	void shutdown_fluid()
	{
		if (impl::impl::has_fluid_state())
		{
			auto _mstate = &impl::main_fluid_state();
			impl::shutdown_fluid(_mstate);
		};
	};
};



namespace fluid::impl
{


	FluidEntity new_entity(FluidState& _fstate)
	{
		return _fstate.ecs.new_entity();
	};
	void destroy_entity(FluidState& _fstate, FluidEntity& _entity)
	{
		_fstate.ecs.destroy_entity(_entity);
	};




	template <ComponentType Type>
	static auto& get_system(FluidState& _fstate)
	{
		return _fstate.csystems.get<component_type_t<Type>>();
	};
	template <ComponentType Type>
	static auto& get_component(FluidState& _fstate, FluidEntity _entity)
	{
		auto& _system = get_system<Type>(_fstate);
		assert(_system->contains(_entity));
		return _system->at(_entity);
	};


	static inline auto& get_system(FluidState& _fstate, ComponentType _type)
	{
		return *(_fstate.ecs.begin() + (size_t)_type);
	};


	void add_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		assert(!_system->contains(_entity));
		_system->insert(_entity);
	};
	bool has_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		return _system->contains(_entity);
	};
	void remove_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		_system->erase(_entity);
	};


};

namespace fluid::impl
{
	bool execute_script(FluidState& _fstate, FluidEntity _entity)
	{
		assert(has_component(_fstate, _entity, ctScript));
		auto& _comp = get_component<ctScript>(_fstate, _entity);
		return _comp.reload();
	};

	void set_script_path(FluidState& _fstate, FluidEntity _entity, const std::filesystem::path& _path)
	{
		assert(has_component(_fstate, _entity, ctScript));
		auto& _system = _fstate.csystems.get<Script>();
		auto& _component = _system->at(_entity);
		_component.source().path = _path;

		auto& _lua = _component.lua();
		
		lua_setglobal(_lua, "fluid");
	};
};
namespace fluid::impl::lua
{

};



namespace fluid
{
	namespace impl
	{
		std::vector<FluidEntity> get_elements(FluidState& _fstate)
		{
			auto& _system = get_system<ctElement>(_fstate);
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

		void set_element_name(FluidState& _fstate, FluidEntity _entity, const std::string& _name)
		{
			auto& _comp = get_component<ctElement>(_fstate, _entity);
			_comp.name = _name;
		};
		std::string get_element_name(FluidState& _fstate, FluidEntity _entity)
		{
			const auto& _comp = get_component<ctElement>(_fstate, _entity);
			return _comp.name;
		};
	};
	
	namespace lua
	{

	};
};

