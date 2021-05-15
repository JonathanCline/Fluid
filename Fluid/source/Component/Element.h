#pragma once

#include "ECS/ECS.h"

#include <string>

namespace PROJECT_NAMESPACE
{
	class Element
	{
	public:
		std::string name;
	};

	class Element_ComponentSystem : public ComponentSystem<Element>
	{
	public:
		void update() override;
	};

	template <>
	struct component_system_type<Element>
	{
		using type = Element_ComponentSystem;
	};

};
