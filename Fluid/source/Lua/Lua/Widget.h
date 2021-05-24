#pragma once

#include "Fluid.h"
#include "FluidState.h"

#include "LuaUtility.h"

#include "Lua/FluidLib.h"



#include <iostream>

namespace fluid::lua
{
	static void log_error(const std::string_view& _message)
	{
		std::cout << _message << '\n';
	};

	struct WidgetLib
	{
	private:
		
		static int close(lua_State* _lua)
		{
			close_widget(FluidLib::lua_toentity_id(_lua, 1));
			return 0;
		};
		static int on(lua_State* _lua)
		{
			const auto _entity = FluidLib::lua_toentity_id(_lua, 1);
			const auto _event = std::string{ luaL_checkstring(_lua, 2) };
			luaL_checktype(_lua, 3, LUA_TFUNCTION);

			{
				lua_assert(has_component(_entity, ctScript));
				auto& _script = get_component<ctScript>(_entity);
				if (!_script.is_defined("FluidWidgetCallbacks"))
				{
					auto& _lua = _script.lua();
					lua_newtable(_lua);
					const auto _cbIndex = lua_gettop(_lua);

					push(_lua, _entity);
					
					{
						lua_newtable(_lua);
						const auto _idx = lua_gettop(_lua);

						constexpr static auto EVENT_NAME = "close";

						// Add the callback
						push(_lua, EVENT_NAME);
						push(_lua);
						lua_copy(_lua, 3, -1);
						lua_settable(_lua, _idx);
					};
					lua_settable(_lua, _cbIndex);
					lua_pushcclosure(_lua, [](lua_State* _lua) -> int
						{
							const auto _idx = lua_upvalueindex(1);
							push(_lua);
							lua_copy(_lua, _idx, -1);
							return 1;
						}, 1);
					lua_setglobal(_lua, "FluidWidgetCallbacks");
				}
				else
				{
					auto& _lua = _script.lua();
					const auto _pretop = lua_gettop(_lua);
					_script.invoke("FluidWidgetCallbacks", 0);
					push(_lua, _entity);
					lua_gettable(_lua, -2);

					push(_lua);
					lua_copy(_lua, 3, -1);
					lua_setfield(_lua, -2, _event.c_str());

					lua_settop(_lua, _pretop);
				};
			};

			if (_event == "close")
			{
				auto& _widget = get_component<ctWidget>(_entity);
				_widget.on_close = [](fluid::FluidEntity _entity)
				{
					assert(fluid::has_component(_entity, ctScript));
					auto& _script = get_component<ctScript>(_entity);
					auto& _lua = _script.lua();

					const auto _pretop = lua_gettop(_lua);
					_script.invoke("FluidWidgetCallbacks", 0);

					if (lua_istable(_lua, -1))
					{
						lua_push(_lua, _entity);
						lua_gettable(_lua, -2);
						if (!lua_istable(_lua, -1))
						{
							goto endoflambda; // prevents the need for me to write good code
						};

						lua_getfield(_lua, -1, "close");
						if (lua_isfunction(_lua, -1))
						{
							lua_pushinteger(_lua, _entity);
							const auto _result = lua_pcall(_lua, 1, 0, 0);
							if (_result != LUA_OK)
							{
								log_error(luaL_optstring(_lua, -1, "lua error!"));
								lua_pop(_lua, 1);
							};
						};
					};
					endoflambda:
					lua_settop(_lua, _pretop);
				};
			}
			else
			{
				lua_pushnil(_lua);
				return 1;
			};

			lua_pushboolean(_lua, true);
			return 1;
		};
		
		constexpr static luaL_Reg luafuncs[] =
		{
			{ "on", on },
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
