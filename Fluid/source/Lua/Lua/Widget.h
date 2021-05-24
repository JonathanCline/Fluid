#pragma once

#include "Fluid.h"
#include "FluidState.h"

#include "LuaUtility.h"

#include "Lua/FluidLib.h"

namespace fluid::lua
{
	struct WidgetLib
	{
	private:
		
		static int close(lua_State* _lua)
		{
			auto& _csys = *fluid::get_system<ctWidget>();
			_csys.close(FluidLib::lua_toentity_id(_lua, 1));
			return 0;
		};
		
		constexpr static luaL_Reg luafuncs[] =
		{
			{ "close", close },
			{ NULL, NULL }
		};

	public:
		constexpr static auto libname = "widget";
		static int openlib(lua_State* _lua)
		{
			luaL_newlibtable(_lua, luafuncs);
			luaL_setfuncs(_lua, luafuncs, 0);
			return 1;
		};

	};

};
