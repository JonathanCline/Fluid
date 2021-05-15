#include <Fluid.h>

#include <SAELib_Timer.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main()
{
	auto _fstate = fluid::start_fluid();
	
	sae::timer _timer{ sae::seconds{2} };

	auto& _fld = *_fstate;
	auto _yourMom = fluid::new_entity(_fld);

	fluid::add_component(_fld, _yourMom, fluid::ctScript);
	fluid::set_script_path(_fld, _yourMom, PROJECT_ROOT "elemscript.lua");
	fluid::execute_script(_fld, _yourMom);

	fluid::update(*_fstate);

	auto _elements = fluid::get_elements(_fld);
	std::cout << "Found Elements: \n";
	for (auto& e : _elements)
	{
		std::cout << "  " << e << " : \"" << fluid::get_element_name(_fld, e) << "\"\n";
	};


	_timer.start();
	while (!_timer.finished())
	{
		fluid::update(*_fstate);
		std::this_thread::sleep_for(16ms);
	};
	
	fluid::destroy_entity(_fld, _yourMom);
	fluid::shutdown_fluid(_fstate);
	return 0;
};
