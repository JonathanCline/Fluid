#include <Fluid.h>

#include <SAELib_Timer.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;



int main()
{
	fluid::start_fluid();
	
	sae::timer _timer{ sae::seconds{2} };

	auto _yourMom = fluid::new_entity();

	fluid::add_component(_yourMom, fluid::ctScript);
	fluid::set_script_path(_yourMom, PROJECT_ROOT "elemscript.lua");
	fluid::execute_script(_yourMom);

	auto _elements = fluid::get_elements();
	std::cout << "Found Elements: \n";
	for (auto& e : _elements)
	{
		std::cout << "  " << e << " : \"" << fluid::get_element_name(e) << "\"\n";
	};

	_timer.start();
	while (!_timer.finished())
	{
		fluid::update();
		std::this_thread::sleep_for(16ms);
	};
	
	fluid::destroy_entity(_yourMom);
	fluid::shutdown_fluid();
	return 0;
};
