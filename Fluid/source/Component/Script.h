#pragma once

#include "LuaAPI.h"

#include "ECS/ECS.h"

#include "Utility/Source.h"


#include <jclua/Table.h>
#include <jclua/State.h>
#include <jclua/Ref.h>
#include <jclua/Thread.h>

#include <filesystem>
#include <chrono>

namespace PROJECT_NAMESPACE
{
	namespace lua
	{
		using namespace jc::lua;
	};

	class Script
	{
	public:
		enum State : int
		{
			Error = -1,
			Done = 0,
			Yield
		};

		auto& lua() noexcept { return this->lua_; };
		const auto& lua() const noexcept { return this->lua_; };

		auto& source() noexcept { return this->source_; };
		const auto& source() const noexcept { return this->source_; };

		bool reload();

		State state() const noexcept;
		State resume();

		Script(lua::LuaState _lua);

	private:
		lua::LuaState lua_;
		lua::unique_ref main_;

		std::unique_ptr<Source> source_;
		
		State state_;

	};

	class Script_ComponentSystem : public ComponentSystem<Script>
	{
	public:
		void insert(Entity _e) override;
		void update() override;

	private:
		size_t update_counter_ = 0;
		size_t update_sources_interval_ = 30;
	};

	template <>
	struct component_system_type<Script>
	{
		using type = Script_ComponentSystem;
	};

};
