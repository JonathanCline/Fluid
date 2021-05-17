#pragma once

#include <string>
#include <filesystem>
#include <cstdint>

namespace fluid
{
	struct FluidState;

	FluidState* start_fluid();
	void shutdown_fluid(FluidState*& _fluid);

	void update(FluidState& _fluid);

};

namespace fluid
{
	using FluidEntity = uint32_t;

	FluidEntity new_entity(FluidState& _fstate);
	void destroy_entity(FluidState& _fstate, FluidEntity& _entity);





	enum ComponentType
	{
		ctScript,
		ctElement,
		ctForm
	};
	
	void add_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);
	bool has_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);
	void remove_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type);



	void set_script_path(FluidState& _fstate, FluidEntity _entity, const std::filesystem::path& _path);
	bool execute_script(FluidState& _fstate, FluidEntity _entity);




	std::vector<FluidEntity> get_elements(FluidState& _fstate);

	void set_element_name(FluidState& _fstate, FluidEntity _entity, const std::string& _name);
	std::string get_element_name(FluidState& _fstate, FluidEntity _entity);



};

