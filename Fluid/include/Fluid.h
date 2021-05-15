#pragma once

namespace fluid
{
	struct FluidState;

	FluidState* start_fluid();
	void shutdown_fluid(FluidState*& _fluid);
};