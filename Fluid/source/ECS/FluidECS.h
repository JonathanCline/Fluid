#pragma once

#include "ECS.h"

#include "Component/Element.h"
#include "Component/Script.h"

#include "Utility/Type.h"



namespace PROJECT_NAMESPACE
{
	/**
	 * @brief List of component typenames
	*/
	using component_typelist = std::tuple<
		Script,
		Element
	>;







	template <ComponentType T> requires ((size_t)T < std::tuple_size_v<component_typelist>)
		struct component_type
	{
		using type = std::tuple_element_t<T, component_typelist>;
	};
	template <ComponentType T> requires requires () { typename component_type<T>::type; }
	using component_type_t = typename component_type<T>::type;

	template <sae::cx_tuple_element<component_typelist> T>
	struct component_enum
	{
		constexpr static auto value = (ComponentType)tuple_element_index_v<component_typelist, T>;
	};
	template <type_in<component_typelist> T>
	constexpr static auto component_enum_v = component_enum<T>::value;

	namespace impl
	{
		template <cx_component... Ts>
		using system_typelist_t = std::tuple<std::add_pointer_t<component_system_type_t<Ts>>... >;

		template <cx_component... Ts>
		consteval static auto system_typelist_helper(const std::tuple<Ts...>& _tup)
		{
			return system_typelist_t<Ts...>{};
		};
	};
	// List of component system types for the ECS
	using component_system_typelist = decltype(impl::system_typelist_helper(std::declval<const component_typelist&>()));

	class FluidECS
	{
	public:
		using system_tup = component_system_typelist;

	private:
		template <typename T>
		void insert_single(EntityComponentSystem& _ecs, T& _system)
		{
			using value_type = std::remove_pointer_t<T>;
			assert(_system == nullptr);
			_system = _ecs.emplace(new value_type{});
		};
		template <typename... Ts>
		void insert_systems(EntityComponentSystem& _ecs, std::tuple<Ts...>& _tup)
		{
			(this->insert_single<Ts>(_ecs, std::get<Ts>(_tup)), ...);
		};

	public:
		void add_to_ecs(EntityComponentSystem& _ecs)
		{
			this->insert_systems(_ecs, this->systems);
		};

		template <cx_component T>
		auto& get()
		{
			return std::get<std::add_pointer_t<component_system_type_t<T>>>(this->systems);
		};
		template <cx_component T>
		const auto& get() const
		{
			return std::get<std::add_pointer_t<component_system_type_t<T>>>(this->systems);
		};


		system_tup systems{};
	};

};