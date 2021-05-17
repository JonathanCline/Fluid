#include "Script.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
#include <ranges>

namespace PROJECT_NAMESPACE
{
	namespace fs = std::filesystem;

	void log_error(const std::string& _msg)
	{
		std::cout << _msg << '\n';
	};

	std::string File::read_all()
	{
		const auto& _path = this->path;

		assert(fs::exists(_path));
		std::ifstream _fstr{ _path };
		assert(_fstr.is_open());

		char _rbuff[512]{ 0 };
		std::ranges::fill(_rbuff, 0);

		std::string _out{};
		while (!_fstr.eof())
		{
			_fstr.read(_rbuff, sizeof(_rbuff));
			const auto _count = _fstr.gcount();
			_out.append(_rbuff, _count);
		};
		return _out;
	};

	Script::State Script::state() const noexcept
	{
		return this->state_;
	};
	bool Script::reload()
	{
		auto& _lua = this->lua();
		assert(_lua);

		const auto _sourceData = this->source().read_all();
		const auto _pretop = lua_gettop(_lua);
		const auto _result = luaL_loadstring(_lua, _sourceData.c_str());

		switch (_result)
		{
		case LUA_OK:
			assert(lua_isfunction(_lua, -1));
			this->main_ = lua::unique_ref{ this->lua() };
			this->state_ = State::Yield;
			break;
		default: // load error
			const auto _message = luaL_optstring(_lua, -1, nullptr);
			if (_message)
			{
				log_error(_message);
			};
			this->state_ = State::Error;
			std::terminate();
			break;
		};
		lua_settop(_lua, _pretop);

		return this->state_ == State::Yield;
	};
	Script::State Script::resume()
	{
		if (this->state() == State::Yield)
		{
			auto& _lua = this->lua();
			assert(_lua);
			
			lua::lua_getref(this->main_);

			int _stackDiff = 0;
			const auto _result = lua_resume(_lua, NULL, 0, &_stackDiff);
			switch (_result)
			{
			case LUA_OK:
				this->state_ = State::Done;
				this->main_.reset();
				break;
			case LUA_YIELD:
				this->state_ = State::Yield;
				break;
			default: // load error
				const auto _message = luaL_optstring(_lua, -1, nullptr);
				if (_message)
				{
					log_error(_message);
				};
				this->state_ = State::Error;
				break;
			};
		};
		return this->state();
	};





	Script::Script(lua::LuaState _lua) :
		lua_{ std::move(_lua) }, main_{ this->lua_, lua::nilref },
		state_{ State::Done }
	{};

};


namespace PROJECT_NAMESPACE
{

	void Script_ComponentSystem::insert(Entity _e)
	{
		this->set_component(_e, Script{ lua::LuaState{ luaL_newstate() } });
		auto& _script = this->at(_e);
		auto& _lua = _script.lua();
		lua::open_default_libs(_lua);
	};
	void Script_ComponentSystem::update()
	{
		for (auto& _scripted : *this)
		{
			auto& [_entity, _script] = _scripted;
			if (_script.state() == _script.Yield)
			{
				_script.resume();
			};
		};
	};

};
