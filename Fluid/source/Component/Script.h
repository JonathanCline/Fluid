#pragma once

#include "ECS/ECS.h"

#include <jclua/Table.h>
#include <jclua/State.h>
#include <jclua/Ref.h>
#include <jclua/Thread.h>

#include <filesystem>

namespace PROJECT_NAMESPACE
{
	namespace lua
	{
		using namespace jc::lua;
	};

	using Path = std::filesystem::path;
	struct File
	{
		std::string read_all();
		Path path;
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

		State state() const noexcept;
		bool reload();
		State resume();


		Script(lua::LuaState _lua);

	private:
		lua::LuaState lua_;
		lua::unique_ref main_;

		File source_;
		State state_;
	};

	class Script_ComponentSystem : public ComponentSystem<Script>
	{
	public:
		void insert(Entity _e) override;
		void update() override;

	private:

	};

	template <>
	struct component_system_type<Script>
	{
		using type = Script_ComponentSystem;
	};

};
