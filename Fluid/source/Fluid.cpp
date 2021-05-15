#include "Fluid.h"

#include "Utility/Singleton.h"
#include "Utility/Pimpl.h"

#include "Window/Window.h"

#include "ECS/ECS.h"
#include "Component/Script.h"
#include "Component/Element.h"


#include <SAELib_Type.h>
#include <SAELib_TupleIndex.h>


#include <GLFW/glfw3.h>


#include <optional>
#include <tuple>
#include <utility>
#include <concepts>
#include <type_traits>
#include <array>


#define fluidns PROJECT_NAMESPACE



namespace fluid
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
		constexpr static auto value = (ComponentType)sae::tuple_element_index_v<component_typelist, T>;
	};
	template <sae::cx_tuple_element<component_typelist> T>
	constexpr static auto component_enum_v = component_enum<T>::value;



	namespace impl
	{
		template <cx_component... Ts>
		using system_typelist_t = std::tuple<std::add_pointer_t<component_system_type_t<Ts>>... > ;

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
			this->insert_systems(_ecs,  this->systems);
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

namespace fluidns
{
	struct FluidState
	{
		EntityComponentSystem ecs{};
		FluidECS csystems{};

		Window main_window{};

		FluidState()
		{
			this->csystems.add_to_ecs(this->ecs);
		};
	};

	FluidState* start_fluid()
	{
		return new FluidState{};
	};
	void shutdown_fluid(FluidState*& _fluid)
	{
		delete _fluid;
		_fluid = nullptr;
	};

	void update(FluidState& _fluid)
	{
		_fluid.ecs.update();
	};


	



};

namespace fluid
{


	FluidEntity new_entity(FluidState& _fstate)
	{
		return _fstate.ecs.new_entity();
	};
	void destroy_entity(FluidState& _fstate, FluidEntity& _entity)
	{
		_fstate.ecs.destroy_entity(_entity);
	};








	static inline auto& get_system(FluidState& _fstate, ComponentType _type)
	{
		return *(_fstate.ecs.begin() + (size_t)_type);
	};


	void add_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		assert(!_system->contains(_entity));
		_system->insert(_entity);
	};
	bool has_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		return _system->contains(_entity);
	};
	void remove_component(FluidState& _fstate, FluidEntity _entity, ComponentType _type)
	{
		auto& _system = get_system(_fstate, _type);
		_system->erase(_entity);
	};

	void set_script_path(FluidState& _fstate, FluidEntity _entity, const std::filesystem::path& _path)
	{
		assert(has_component(_fstate, _entity, ctScript));
		auto& _system = _fstate.csystems.get<Script>();
		auto& _component = _system->at(_entity);
		_component.source().path = _path;
		auto _good = _component.reload();
		assert(_good);
	};


};


