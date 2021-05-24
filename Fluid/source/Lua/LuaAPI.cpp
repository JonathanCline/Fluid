#include "LuaAPI.h"

#include "Lua/FluidLib.h"
#include "Lua/Package.h"
#include "Lua/Widget.h"

#include <lua.hpp>


#include <iostream>

namespace PROJECT_NAMESPACE::lua
{
	namespace impl
	{
		void destroy_luastate(lua_State*& _lua)
		{
			lua_close(_lua);
			_lua = nullptr;
		};
	};

	// Needs a single upvalue containing a std::ostream* to write to
	int luafunc_print(lua_State* _lua)
	{
		// Get output stream upvalue
		const auto _ostrPtr = (std::ostream*)(lua_touserdata(_lua, lua_upvalueindex(1)));
		assert(_ostrPtr);
		
		// Write values
		auto& _ostr = *_ostrPtr;
		const auto _args = lua_gettop(_lua);
		for (int i = 1; i <= _args; ++i)
		{
			_ostr << lua_tostring(_lua, i) << " ";
		};
		_ostr << std::endl;
		return 0;
	};
	int luafunc_pairs(lua_State* _lua)
	{
		lua_assert(lua_istable(_lua, 1));
		lua_pushnil(_lua);
		lua_pushcclosure(_lua, [](lua_State* _lua) -> int
			{
				const auto _tableIndex = lua_upvalueindex(1);
				const auto _keyIndex = lua_upvalueindex(2);
				
				// Push key onto top
				push(_lua);
				lua_copy(_lua, _keyIndex, -1);
				
				// Get next key value pair
				const auto _deltaTop = lua_next(_lua, _tableIndex);
				if (_deltaTop != 0)
				{
					// Stack looks like :
					//	[-2] key: string
					//  [-1] value: any

					// Set next key and remove from stack
					lua_copy(_lua, -2, _keyIndex);
					return 2;
				}
				else
				{
					// If we reached the end, return nil
					push(_lua);
					return 1;
				};
			}, 2);
		return 1;
	};
	int luafunc_tonumber(lua_State* _lua)
	{
		if (luaL_callmeta(_lua, 1, "__tonumber") == LUA_TNIL)
		{
			push(_lua, lua_tonumber(_lua, 1));
		};
		lua_assert(lua_isnumber(_lua, -1));
		return 1;
	};
	int luafunc_round(lua_State* _lua)
	{
		push(_lua, (lua_Integer)std::round(pull<lua_Number>(_lua, index_abs{ 1 })));
		return 1;
	};


	void open_default_libs(const LuaState& _lua)
	{

		lua_pushlightuserdata(_lua, static_cast<std::ostream*>(&std::cout));	
		lua_pushcclosure(_lua, luafunc_print, 1);
		lua_setglobal(_lua, "print");

		push(_lua, luafunc_pairs);
		lua_setglobal(_lua, "pairs");

		push(_lua, luafunc_tonumber);
		lua_setglobal(_lua, "tonumber");


		push(_lua, luafunc_round);
		lua_setglobal(_lua, "round");


		// Open package library
		{
			const auto _deltaIdx = Package::openlib(_lua);
			assert(_deltaIdx == 1);
		};

		// Open the rest of the libraries using package library
		require(_lua, Lib_Fluid::libname, Lib_Fluid::openlib);


		require(_lua, WidgetLib::libname, WidgetLib::openlib);
		
	};


};
