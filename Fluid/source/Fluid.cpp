#include "Fluid.h"

#include "Window/Window.h"

#include "ECS/ECS.h"
#include "Component/Script.h"
#include "Component/Element.h"

#include "Utility/Memory.h"
#include "Utility/Singleton.h"
#include "Utility/Pimpl.h"


#include <SAELib_Type.h>
#include <SAELib_TupleIndex.h>


#include <GLFW/glfw3.h>


#include <optional>
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>
#include <array>

namespace fluid
{
	/**
	 * @brief List of component typenames
	*/
	using component_typelist = std::tuple<
		Script,
		Element
	>;

	






	template <ComponentType T> requires ((size_t)T < std::tuple_size_v<component_typelist>)
		struct component_type
	{
		using type = std::tuple_element_t<T, component_typelist>;
	};
	template <ComponentType T> requires requires () { typename component_type<T>::type; }
	using component_type_t = typename component_type<T>::type;

	template <sae::cx_tuple_element<component_typelist> T>
	struct component_enum
	{
		constexpr static auto value = (ComponentType)sae::tuple_element_index_v<component_typelist, T>;
	};
	template <sae::cx_tuple_element<component_typelist> T>
	constexpr static auto component_enum_v = component_enum<T>::value;



	namespace impl
	{
		template <cx_component... Ts>
		using system_typelist_t = std::tuple<std::add_pointer_t<component_system_type_t<Ts>>... > ;

		template <cx_component... Ts>
		consteval static auto system_typelist_helper(const std::tuple<Ts...>& _tup)
		{
			return system_typelist_t<Ts...>{};
		};
	};
	// List of component system types for the ECS
	using component_system_typelist = decltype(impl::system_typelist_helper(std::declval<const component_typelist&>()));
	
	class FluidECS
	{
	public:
		using system_tup = component_system_typelist;
		
	private:
		template <typename T>
		void insert_single(EntityComponentSystem& _ecs, T& _system)
		{
			using value_type = std::remove_pointer_t<T>;
			assert(_system == nullptr);
			_system = _ecs.emplace(new value_type{});
		};
		template <typename... Ts>
		void insert_systems(EntityComponentSystem& _ecs, std::tuple<Ts...>& _tup)
		{
			(this->insert_single<Ts>(_ecs, std::get<Ts>(_tup)), ...);
		};

	public:
		void add_to_ecs(EntityComponentSystem& _ecs)
		{
			this->insert_systems(_ecs,  this->systems);
		};
		
		template <cx_component T>
		auto& get()
		{
			return std::get<std::add_pointer_t<component_system_type_t<T>>>(this->systems);
		};
		template <cx_component T>
		const auto& get() const
		{
			return std::get<std::add_pointer_t<component_system_type_t<T>>>(this->systems);
		};


		system_tup systems{};
	};

};

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
namespace fluid::impl::lua
{
	struct Lib_Fluid
	{
	private:
		constexpr static const char* entity_typename = "Fluid.Type.Entity";

		static fluid::FluidState& get_fstate(lua_State* _lua)
		{
			auto _fstate = (FluidState*)lua_touserdata(_lua, lua_upvalueindex(1));
			lua_assert(_fstate);
			return *_fstate;
		};
		static fluid::Entity& lua_toentity(lua_State* _lua, int _atIndex)
		{
			auto _out = (fluid::Entity*)luaL_checkudata(_lua, lua_absindex(_lua, _atIndex), entity_typename);
			lua_assert(_out);
			return *_out;
		};

		static int new_entity(lua_State* _lua)
		{
			auto& _fstate = get_fstate(_lua);
			auto _entity = fluid::impl::new_entity(_fstate);

			auto _memory = (decltype(_entity)*)lua_newuserdata(_lua, sizeof(_entity));
			*_memory = _entity;
			luaL_setmetatable(_lua, entity_typename);

			return 1;
		};
		static int destroy_entity(lua_State* _lua)
		{
			auto& _fstate = get_fstate(_lua);
			auto& _entity = lua_toentity(_lua, 1);
			fluid::impl::destroy_entity(_fstate, _entity);
			return 0;
		};



		
		static int has_component(lua_State* _lua)
		{
			auto& _fstate = get_fstate(_lua);
			auto& _entity = lua_toentity(_lua, 1);
			const auto _type = luaL_checkinteger(_lua, 2);
			lua_pushboolean(_lua, fluid::impl::has_component(_fstate, _entity, (ComponentType)_type));
			return 1;
		};

		constexpr static luaL_Reg entityfuncs[] =
		{
			{ "new", new_entity },
			{ "has", has_component },
			{ "__gc", destroy_entity },
			{ NULL, NULL }
		};



		static int add_component(lua_State* _lua)
		{
			auto& _fstate = get_fstate(_lua);
			auto& _entity = lua_toentity(_lua, 1);
			const auto _type = (ComponentType)luaL_checkinteger(_lua, 2);
			fluid::impl::add_component(_fstate, _entity, _type);
			return 0;
		};

		static int yield(lua_State* _lua)
		{
			return lua_yield(_lua, 0);
		};

		constexpr static luaL_Reg libfuncs[] =
		{
			{ "add_component", add_component },
			{ "yield", yield },
			{ NULL, NULL }
		};

	public:
		static int openlib(lua_State* _lua, FluidState* _fstate)
		{
			luaL_newlibtable(_lua, libfuncs);
			const auto _idx = lua_gettop(_lua);

			lua_pushlightuserdata(_lua, _fstate);
			luaL_setfuncs(_lua, libfuncs, 1);


			// push component type enumerators

			lua_pushinteger(_lua, (lua_Integer)ctScript);
			lua_setfield(_lua, _idx, "ctScript");

			lua_pushinteger(_lua, (lua_Integer)ctElement);
			lua_setfield(_lua, _idx, "ctElement");


			lua_newtable(_lua);
			const auto _component = lua_gettop(_lua);

			{
				lua_newtable(_lua);
				const auto _element = lua_gettop(_lua);
				
				lua_pushlightuserdata(_lua, _fstate);
				lua_pushcclosure(_lua, [](lua_State* _lua) -> int
					{
						auto& _fstate = get_fstate(_lua);
						auto& _entity = lua_toentity(_lua, 1);
						const auto& _name = get_component<ctElement>(_fstate, _entity).name;
						lua_pushstring(_lua, _name.c_str());
						return 1;
					}, 1);
				lua_setfield(_lua, _element, "get_name");

				lua_pushlightuserdata(_lua, _fstate);
				lua_pushcclosure(_lua, [](lua_State* _lua) -> int
					{
						auto& _fstate = get_fstate(_lua);
						auto& _entity = lua_toentity(_lua, 1);
						const auto _name = luaL_checkstring(_lua, 2);
						fluid::impl::set_element_name(_fstate, _entity, _name);
						return 0;
					}, 1);
				lua_setfield(_lua, _element, "set_name");
			};
			lua_settop(_lua, _component + 1);
			lua_setfield(_lua, _component, "element");

			{
				lua_newtable(_lua);
				const auto _script = lua_gettop(_lua);
			};
			lua_settop(_lua, _component + 1);
			lua_setfield(_lua, _component, "script");

			lua_setfield(_lua, _idx, "component");


			// add entity userdata type

			luaL_newmetatable(_lua, entity_typename);
			const auto _entityIdx = lua_gettop(_lua);
			for (auto& r : entityfuncs)
			{
				if (r.func && r.name)
				{
					lua_pushlightuserdata(_lua, _fstate);
					lua_pushcclosure(_lua, r.func, 1);
					lua_setfield(_lua, _entityIdx, r.name);
				};
			};
			lua_setfield(_lua, _idx, "entity");

			return 1;
		};
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
		lua::Lib_Fluid::openlib(_lua, &_fstate);
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

