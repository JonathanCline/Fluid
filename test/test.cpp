#include <Fluid.h>

#include <SAELib_Timer.h>

#include <lua.hpp>

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

extern inline bool KEEP_ALIVE = true;











int main()
{

	
	fluid::start_fluid();
	const auto _window = fluid::get_main_window();
	fluid::set_widget_close_callback(_window, [](fluid::FluidEntity _window)
		{
			KEEP_ALIVE = false;
		});

	sae::timer _timer{ sae::milliseconds{ 16 } };

	fluid::add_component(_window, fluid::ctScript);
	fluid::set_script_path(_window, TEST_SCRIPT_PATH, true);
	fluid::reload_script(_window);

	while (KEEP_ALIVE)
	{
		_timer.start();

		// Run update
		fluid::update();

		if (!_timer.finished())
		{
			std::this_thread::sleep_for(_timer.remaining_time());
		};
	};
	
	fluid::shutdown_fluid();
	return 0;
};
