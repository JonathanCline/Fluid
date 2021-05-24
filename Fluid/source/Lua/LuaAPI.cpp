#include "LuaAPI.h"

#include "Lua/FluidLib.h"
#include "Lua/Package.h"

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

	void open_default_libs(const LuaState& _lua)
	{
		lua_pushlightuserdata(_lua, static_cast<std::ostream*>(&std::cout));
		lua_pushcclosure(_lua, luafunc_print, 1);
		lua_setglobal(_lua, "print");
		
		// Open package library
		{
			const auto _deltaIdx = Package::openlib(_lua);
			assert(_deltaIdx == 1);
		};

		// Open the rest of the libraries using package library
		require(_lua, Lib_Fluid::libname, Lib_Fluid::openlib);
		
	};


};
