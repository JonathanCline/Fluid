#pragma once

#include "Fluid.h"

#include "FluidState.h"

#include "ECS/FluidECS.h"
#include "Component/Element.h"



#include "Lua/LuaUtility.h"
#include <lua.hpp>


#include <string_view>
#include <array>
#include <ranges>
#include <cassert>

namespace PROJECT_NAMESPACE::lua
{
	using namespace std::string_view_literals;



	struct Lib_Fluid
	{
	private:
		constexpr static const char* entity_typename = "Fluid.TEntity";

	public:
		static auto& lua_toentity(lua_State* _lua, int _atIndex)
		{
			auto _out = (fluid::FluidEntity*)luaL_checkudata(_lua, lua_absindex(_lua, _atIndex), entity_typename);
			lua_assert(_out);
			return *_out;
		};
		static auto lua_toentity_id(lua_State* _lua, int _index)
		{
			if (lua_isinteger(_lua, _index))
			{
				return (fluid::FluidEntity)lua_tointeger(_lua, _index);
			}
			else
			{
				return lua_toentity(_lua, _index);
			};
		};

	private:
		static int new_entity(lua_State* _lua)
		{
			auto _entity = fluid::new_entity();

			using type = decltype(_entity);
			auto _memory = (type*)lua_newuserdata(_lua, sizeof(_entity));
			*_memory = _entity;
			luaL_setmetatable(_lua, entity_typename);
			return 1;
		};
		static int destroy_entity(lua_State* _lua)
		{
			auto& _entity = lua_toentity(_lua, 1);
			fluid::destroy_entity(_entity);
			return 0;
		};

		static int has_component(lua_State* _lua)
		{
			auto& _entity = lua_toentity(_lua, 1);
			const auto _type = luaL_checkinteger(_lua, 2);
			lua_pushboolean(_lua, fluid::has_component(_entity, (fluid::ComponentType)_type));
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
			auto& _entity = lua_toentity(_lua, 1);
			const auto _type = (fluid::ComponentType)luaL_checkinteger(_lua, 2);
			fluid::add_component(_entity, _type);
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
		constexpr static auto libname = "fluid";
		static int openlib(lua_State* _lua)
		{
			luaL_newlibtable(_lua, libfuncs);
			const auto _idx = lua_gettop(_lua);
			luaL_setfuncs(_lua, libfuncs, 0);


			// push component type enumerators

			lua_pushinteger(_lua, (lua_Integer)ctScript);
			lua_setfield(_lua, _idx, "ctScript");

			lua_pushinteger(_lua, (lua_Integer)ctElement);
			lua_setfield(_lua, _idx, "ctElement");

			lua_pushinteger(_lua, (lua_Integer)ctWidget);
			lua_setfield(_lua, _idx, "ctWidget");






			lua_newtable(_lua);
			const auto _component = lua_gettop(_lua);

			// add entity userdata type
			luaL_newmetatable(_lua, entity_typename);
			const auto _entityIdx = lua_gettop(_lua);
			for (auto& r : entityfuncs)
			{
				if (r.func && r.name)
				{
					lua_pushcfunction(_lua, r.func);
					lua_setfield(_lua, _entityIdx, r.name);
				};
			};
			lua_setfield(_lua, _idx, "entity");

			// Element sublibrary
			lua_newtable(_lua);
			lua_pushcfunction(_lua, [](lua_State* _lua) -> int
				{
					lua_newtable(_lua);
					const auto _idx = lua_gettop(_lua);
					
					// Push elements
					auto& _system = *fluid_state().csystems.get<Element>();
					for (auto& e : _system.container())
					{
						push(_lua, e.first);
						push(_lua, e.second.name);
						lua_settable(_lua, _idx);
					};
					return 1;
				});
			lua_setfield(_lua, -2, "list");
			lua_setfield(_lua, _idx, "element");

			return 1;
		};
	};
	using FluidLib = Lib_Fluid;

};