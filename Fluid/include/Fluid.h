#pragma once

#include <string>
#include <filesystem>
#include <cstdint>
#include <utility>

namespace fluid
{
	struct FluidState;

	namespace impl
	{
		FluidState* start_fluid();
		void shutdown_fluid(FluidState*& _fluid);
		void update(FluidState& _fluid);

		FluidState& main_fluid_state();

		template <typename FunctionT, typename... ArgTs> requires std::invocable<FunctionT, FluidState&, ArgTs...>
		constexpr static auto invoke_with_main(const FunctionT& _func, ArgTs&&... _args)
		{
			return std::invoke(_func, main_fluid_state(), std::forward<ArgTs>(_args)...);
		};
	};

	static FluidState* start_fluid()
	{
		return impl::start_fluid();
	};
	void shutdown_fluid();

	static void update()
	{
		return impl::invoke_with_main(impl::update);
	};

};

namespace fluid
{
	using FluidEntity = uint32_t;

	namespace impl
	{
		FluidEntity new_entity(FluidState& _fstate);
		void destroy_entity(FluidState& _fstate, FluidEntity& _entity);
	};

	static FluidEntity new_entity()
	{
		return impl::invoke_with_main(impl::new_entity);
	};
	static void destroy_entity(FluidEntity& _entity)
	{
		return impl::invoke_with_main(impl::destroy_entity, _entity);
	};




	enum ComponentType
	{
		ctScript,
		ctElement
	};
	
	namespace impl
	{
		void add_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);
		bool has_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);
		void remove_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);
	};

	static void add_component(FluidEntity _entity, ComponentType _type)
	{
		return impl::invoke_with_main(impl::add_component, _entity, _type);
	};
	static bool has_component(FluidEntity _entity, ComponentType _type)
	{
		return impl::invoke_with_main(impl::has_component, _entity, _type);
	};
	static void remove_component(FluidEntity _entity, ComponentType _type)
	{
		return impl::invoke_with_main(impl::remove_component, _entity, _type);
	};

	



	namespace impl
	{
		void set_script_path(FluidState& _fstate, FluidEntity _entity, const std::filesystem::path& _path);
		bool execute_script(FluidState& _fstate, FluidEntity _entity);
	};

	static void set_script_path(FluidEntity _entity, const std::filesystem::path& _path)
	{
		return impl::invoke_with_main(impl::set_script_path, _entity, _path);
	};
	static bool execute_script(FluidEntity _entity)
	{
		return impl::invoke_with_main(impl::execute_script, _entity);
	};



	namespace impl
	{
		std::vector<FluidEntity> get_elements(FluidState& _fstate);
		void set_element_name(FluidState& _fstate, FluidEntity _entity, const std::string& _name);
		std::string get_element_name(FluidState& _fstate, FluidEntity _entity);
	};

	static std::vector<FluidEntity> get_elements()
	{
		return impl::invoke_with_main(impl::get_elements);
	};
	static void set_element_name(FluidEntity _entity, const std::string& _name)
	{
		return impl::invoke_with_main(impl::set_element_name, _entity, _name);
	};
	static std::string get_element_name(FluidEntity _entity)
	{
		return impl::invoke_with_main(impl::get_element_name, _entity);
	};



};

