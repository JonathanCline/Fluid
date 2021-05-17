#pragma once

#include <string>
#include <filesystem>
#include <cstdint>
#include <utility>

namespace fluid
{
	struct FluidState;
	using FluidEntity = uint32_t;

	enum ComponentType
	{
		ctScript,
		ctElement
	};


	bool start_fluid();
	void shutdown_fluid();
	void update();


	FluidEntity new_entity();
	void destroy_entity(FluidEntity& _entity);


	void set_script_path(FluidEntity _entity, const std::filesystem::path& _path);
	bool execute_script(FluidEntity _entity);


	void add_component(FluidEntity _entity, ComponentType _type);
	bool has_component(FluidEntity _entity, ComponentType _type);
	void remove_component(FluidEntity _entity, ComponentType _type);


	std::vector<FluidEntity> get_elements();
	void set_element_name(FluidEntity _entity, const std::string& _name);
	std::string get_element_name(FluidEntity _entity);

};