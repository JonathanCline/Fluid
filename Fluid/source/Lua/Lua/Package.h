#pragma once

#include "LuaUtility.h"

#include <lua.hpp>

namespace fluid::lua
{
	struct Package
	{
	public:
		static void require(lua_State* _lua, const char* _name, int(*_openFunction)(lua_State*), bool _global)
		{
#ifndef NDEBUG
			const auto _initialTop = lua_gettop(_lua);
#endif
			// Get the loaded table
			lua_getglobal(_lua, "loadedRef");
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, luaL_checkinteger(_lua, -1));
			lua_remove(_lua, -2);
			assert(lua_istable(_lua, -1));

			// Run the open function, in the future this should hold off on opening until its time
			const auto _pretop = lua_gettop(_lua);
			const auto _deltaIndex = std::invoke(_openFunction, _lua);
			lua_settop(_lua, _pretop + 1);
			assert(lua_istable(_lua, -1));

			// Set global if bool is set
			if (_global)
			{
				lua_getglobal(_lua, _name);
				lua_copy(_lua, -2, -1);
				lua_setglobal(_lua, _name);
			};
			
			// Add library to loaded table
			lua_setfield(_lua, -2, _name);
			lua_pop(_lua, 1);

			// only adding the compile guard to keep intellisense happy
#ifndef NDEBUG
			// Check that stack is left un-modified
			assert(lua_gettop(_lua) == _initialTop);
#endif
		};
		static void require(lua_State* _lua, const char* _name, int(*_openFunction)(lua_State*))
		{
			return require(_lua, _name, _openFunction, false);
		};

	private:
		static int require(lua_State* _lua)
		{
			const auto _tableIdx = lua_upvalueindex(1);
			assert(lua_istable(_lua, _tableIdx));

			const auto _libname = luaL_checkstring(_lua, 1);
			lua_getfield(_lua, _tableIdx, _libname);
			assert(lua_istable(_lua, -1) || lua_isnil(_lua, -1));
			return 1;
		};

		constexpr static luaL_Reg luafuncs[] =
		{
			{ "require", &Package::require },
			{ NULL, NULL }
		};

	public:
		constexpr static auto libname = "package";
		
		static int openlib(lua_State* _lua)
		{
			// Make loaded table
			lua_newtable(_lua);
			const auto _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
			push(_lua, _ref);
			lua_setglobal(_lua, "loadedRef");
			
			// Make library table, upvalue[0] = loaded table
			luaL_newlibtable(_lua, luafuncs);
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);
			luaL_setfuncs(_lua, luafuncs, 1);

			// Push copy onto top and add to loaded table
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);
			push(_lua);
			lua_copy(_lua, -3, -1);
			lua_setfield(_lua, -2, libname);

			// Make global require function
			lua_rawgeti(_lua, LUA_REGISTRYINDEX, _ref);
			lua_pushcclosure(_lua, require, 1);
			lua_setglobal(_lua, "require");

			return 1;
		};
	
	};

	static void require(lua_State* _lua, const char* _name, int(*_openFunction)(lua_State*), bool _global)
	{
		return Package::require(_lua, _name, _openFunction, _global);
	};
	static void require(lua_State* _lua, const char* _name, int(*_openFunction)(lua_State*))
	{
		return Package::require(_lua, _name, _openFunction);
	};



};
