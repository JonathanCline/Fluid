#pragma once

#include "ECS/ECS.h"

namespace fluid
{
	class Widget
	{
	public:
		using OnClose = void(*)(Entity _entity);
		OnClose on_close = nullptr;
	};

	class Widget_ComponentSystem : public ComponentSystem<Widget>
	{
	public:
		void close(Entity _entity)
		{
			auto& _widget = this->at(_entity);
			if (_widget.on_close)
			{
				std::invoke(_widget.on_close, _entity);
			};
		};
		void update() override {};
	};

	template <>
	struct component_system_type<Widget>
	{
		using type = Widget_ComponentSystem;
	};
};
