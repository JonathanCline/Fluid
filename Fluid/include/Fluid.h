#pragma once

#include <string>
#include <filesystem>
#include <cstdint>
#include <utility>
#include <memory>

namespace fluid
{
	struct FluidState;
	using FluidEntity = uint32_t;

	enum ComponentType
	{
		ctScript,
		ctElement,
		ctWidget,
		ctArt
	};


	bool start_fluid();
	void shutdown_fluid();
	void update();


	FluidEntity new_entity();
	void destroy_entity(FluidEntity& _entity);


	void set_script_path(FluidEntity _entity, const std::filesystem::path& _path, bool _reloadOnChange);
	void set_script_source(FluidEntity _entity, const std::string& _str);
	
	

	bool reload_script(FluidEntity _entity);
	bool run_script(FluidEntity _entity);
	bool resume_script(FluidEntity _entity);



	void add_component(FluidEntity _entity, ComponentType _type);
	bool has_component(FluidEntity _entity, ComponentType _type);
	void remove_component(FluidEntity _entity, ComponentType _type);



	std::vector<FluidEntity> get_elements();
	void set_element_name(FluidEntity _entity, const std::string& _name);
	std::string get_element_name(FluidEntity _entity);


	
	FluidEntity get_main_window();



	using WidgetCloseCallback = void(*)(FluidEntity);
	// call with _callback = nullptr to remove the callback from the widget
	void set_widget_close_callback(FluidEntity _entity, WidgetCloseCallback _callback);
	void close_widget(FluidEntity _entity);





	

	
};

