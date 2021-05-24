#pragma once

#include "Utility/Guard.h"

namespace PROJECT_NAMESPACE
{
#if false
	namespace impl
	{
		using entity_guard = guard < fluid::FluidEntity, guard_traits < fluid::FluidEntity,
			[](fluid::FluidEntity& _e) { _e = 0; },
			[](fluid::FluidEntity& _e) { fluid::destroy_entity(_e); },
			[](const fluid::FluidEntity& _e) -> bool { return _e == 0; } >>;
	};

	class EntityHandle : public impl::entity_guard
	{
	private:
		using parent_type = impl::entity_guard;
	public:
		operator auto& () { return this->get(); };
		operator const auto& () const { return this->get(); };
		using parent_type::parent_type;
	};
#endif
};
