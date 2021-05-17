#include "LuaAPI.h"

#include "Lua/Fluid.h"



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



};
