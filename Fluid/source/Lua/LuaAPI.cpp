#include "LuaAPI.h"

#include "Lua/FluidLib.h"



#include <lua.hpp>

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

	void open_default_libs(const LuaState& _lua)
	{
		luaL_openlibs(_lua);
		luaL_requiref(_lua, "fluid", Lib_Fluid::openlib, false);
	};


};
