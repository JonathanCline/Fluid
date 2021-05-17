#pragma once

#include "Utility/Pimpl.h"

struct lua_State;

namespace PROJECT_NAMESPACE::lua
{
	namespace impl
	{
		void destroy_luastate(lua_State*& _lua);
	};
	
	/**
	 * @brief RAII wrapper for a single lua_State, like unique_ptr but has implicit lua_State* conversion for ease of use
	*/
	class LuaState : public pimpl_ptr<lua_State, impl::destroy_luastate>
	{
	public:
		operator lua_State*& () noexcept
		{
			return this->get();
		};
		operator lua_State* const& () const noexcept
		{
			return this->get();
		};

		using pimpl_ptr<lua_State, impl::destroy_luastate>::pimpl_ptr;
	};


	void open_default_libs(const LuaState& _lua);


};
