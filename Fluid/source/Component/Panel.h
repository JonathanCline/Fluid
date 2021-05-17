#pragma once

#include "ECS/ECS.h"

#include "Utility/Type.h"


#include <queue>
#include <variant>

namespace PROJECT_NAMESPACE
{
	class Panel
	{
	public:
		using ResizeFunction = void (*)(Entity, int, int);
		using MouseFunction = void (*)(Entity, int, int, int);

		void resize(Entity _entity, int _width, int _height)
		{
			if (this->on_resize)
			{
				std::invoke(this->on_resize, _entity, _width, _height);
			};
		};
		void mouse(Entity _entity, int _button, int _action, int _mods)
		{
			if (this->on_mouse)
			{
				std::invoke(this->on_mouse, _entity, _button, _action, _mods);
			};
		};

		ResizeFunction on_resize = nullptr;
		MouseFunction on_mouse = nullptr;
	};




	struct Event_Resize
	{
		int width;
		int height;
	};

	struct Event_Mouse
	{
		int button;
		int action;
		int mods;
	};



	using event_typelist = std::tuple<Event_Resize, Event_Mouse>;

	template <tuple_element<event_typelist> T>
	using EventCallback = void(*)(Entity, const T&);






	class PanelEvent
	{
	public:
		using typelist = std::tuple<Event_Resize, Event_Mouse>;
		enum Type : size_t
		{
			evResize,
			evMouse
		};

		template <tuple_element<typelist> T>
		constexpr static Type type_index_v = (Type)tuple_element_index_v<T>;

		template <Type T>
		using index_type_t = std::tuple_element_t<(size_t)T, typelist>;

	private:
		using variant_type = make_variant_from_t<typelist>;
	
	public:
		Type type() const noexcept { return (Type)this->vt_.index(); };

		template <tuple_element<typelist> T>
		T& get() { return std::get<T>(this->vt_); };
		template <tuple_element<typelist> T>
		const T& get() const { return std::get<T>(this->vt_); };

		template <Type T>
		auto& get()
		{
			return this->get<index_type_t<T>>();
		};
		template <Type T>
		const auto& get() const
		{
			return this->get<index_type_t<T>>();
		};
		
		template <typename T> requires tuple_element<std::remove_cvref_t<T>, typelist>
		PanelEvent(T&& _event) :
			vt_{ std::forward<T>(_event) }
		{};

		variant_type vt_{};
	};

	class Panel_ComponentSystem : public ComponentSystem<Panel>
	{
	public:
		void push_event(const PanelEvent& _event);

		void update() override;

	private:
		std::queue<PanelEvent> events_{};
	};

	template <>
	struct component_system_type<Panel>
	{
		using type = Panel_ComponentSystem;
	};
};
