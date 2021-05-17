#include <Fluid.h>

#include <SAELib_Timer.h>

#include <iostream>
#include <chrono>
#include <thread>

#ifdef assert
#undef assert
#endif
#ifndef NDEBUG
#define assert(cond) if(! ( cond ) ) { std::abort(); }
#else
#define assert(cond) static_assert(true, "")
#endif

using namespace std::chrono_literals;

constexpr auto TEST_SCRIPT_PATH = PROJECT_ROOT "elemscript.lua";

int main()
{
	fluid::start_fluid();

	sae::timer _timer{ sae::seconds{ 60 } };

	auto _yourMom = fluid::new_entity();

	fluid::add_component(_yourMom, fluid::ctScript);
	fluid::set_script_path(_yourMom, TEST_SCRIPT_PATH, true);
	fluid::reload_script(_yourMom);

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
