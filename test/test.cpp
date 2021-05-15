#include <Fluid.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main()
{
	auto _fstate = fluid::start_fluid();
	std::this_thread::sleep_for(3s);
	fluid::shutdown_fluid(_fstate);
	return 0;
};
