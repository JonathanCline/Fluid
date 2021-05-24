#pragma once

#include <SAELib_KeyVector.h>
#include <SAELib_Type.h>

#include <cassert>
#include <cstdint>
#include <vector>
#include <memory>

namespace PROJECT_NAMESPACE
{
	using Entity = uint32_t;
	enum : Entity
	{
		null_entity = 0u
	};

	class EntityFactory
	{
	public:
		using State = Entity;
		
		State get_state() const
		{
			return this->state_;
		};
		void load_state(State _s)
		{
			this->state_ = _s;
		};
		
		Entity new_entity()
		{
			Entity _out = ++this->state_;
			assert(_out != null_entity);
			return _out;
		};
		void recycle_entity(Entity _e)
		{
			// nothing for now, but an important placeholder
		};

		void reset() { this->state_ = 0; };

		EntityFactory()
		{
			this->reset();
		};

	private:
		State state_ = 0;
	};

	class AbstractComponentSystem
	{
	public:
		virtual void insert(Entity _entity) = 0;
		
		virtual void erase(Entity _entity) = 0;
		virtual void erase_range(const std::vector<Entity>& _evec)
		{
			for (auto& e : _evec)
			{
				this->erase(e);
			};
		};

		virtual bool contains(Entity _entity) const = 0;
		
		virtual void update() = 0;
		
		virtual ~AbstractComponentSystem() = default;
	
	};


	namespace impl
	{
		template <typename T, typename ComponentT>
		struct component_helper_dc : public AbstractComponentSystem
		{
		private:
			sae::kvector<Entity, ComponentT>& get_data() { return static_cast<T*>(this)->container(); };
		public:
			void insert(Entity _e) override
			{
				this->get_data().insert({ _e, ComponentT{} });
			};
		};

		template <typename T, typename C>
		using make_csystem_helper = sae::type_switch_t<
			(size_t)std::is_default_constructible_v<C>,
			AbstractComponentSystem,
			component_helper_dc<T, C>
		>;
	};

	template <typename ComponentT>
	class ComponentSystem : public impl::make_csystem_helper<ComponentSystem<ComponentT>, ComponentT>
	{
	public:
		using component_type = ComponentT;
		sae::kvector<Entity, ComponentT>& container()
		{
			return this->data_;
		};

	protected:
		auto begin() noexcept { return this->data_.begin(); };
		auto cbegin() const noexcept { return this->data_.begin(); };
		auto begin() const noexcept { return this->cbegin(); };

		auto end() noexcept { return this->data_.end(); };
		auto cend() const noexcept { return this->data_.end(); };
		auto end() const noexcept { return this->cend(); };

		auto find(Entity _e) noexcept
		{
			return this->container().find(_e);
		};
		const auto find(Entity _e) const noexcept
		{
			return this->container().find(_e);
		};

		void set_component(Entity _e, component_type _component)
		{
			this->data_.insert({ _e, std::move(_component) });
		};

	public:


		// aborts if contains(_e) would return false
		component_type& at(Entity _e)
		{
			return this->data_.at(_e);
		};
		// aborts if contains(_e) would return false
		const component_type& at(Entity _e) const
		{
			return this->data_.at(_e);
		};
		
		auto& operator[](Entity _e) { return this->at(_e); };
		const auto& operator[](Entity _e) const { return this->at(_e); };

		bool contains(Entity _e) const override
		{
			return this->data_.contains(_e);
		};
		void erase(Entity _e) override
		{
			this->data_.erase(_e);
		};
		void erase_range(const std::vector<Entity>& _evec) override
		{
			for (auto& e : _evec)
			{
				this->erase(e);
			};
		};

	private:
		sae::kvector<Entity, ComponentT> data_;
	};


	class EntityComponentSystem
	{
	public:
		auto& factory() noexcept { return this->factory_; };
		const auto& factory() const noexcept { return this->factory_; };

	private:
		AbstractComponentSystem* insert_impl(std::unique_ptr<AbstractComponentSystem> _system);

	public:
		auto begin() noexcept { return this->data_.begin(); };
		auto begin() const noexcept { return this->data_.cbegin(); };
		auto cbegin() const noexcept { return this->data_.cbegin(); };
		
		auto end() noexcept { return this->data_.end(); };
		auto end() const noexcept { return this->data_.cend(); };
		auto cend() const noexcept { return this->data_.cend(); };





		template <std::derived_from<AbstractComponentSystem> T>
		T* insert(std::unique_ptr<T> _system)
		{
			return static_cast<T*>(this->insert_impl(std::move(_system)));
		};
		template <std::derived_from<AbstractComponentSystem> T>
		T* emplace(T* _system)
		{
			return insert(std::unique_ptr<T>{ _system });
		};


		size_t erase(AbstractComponentSystem* _system);


		void update();

		Entity new_entity();
		void destroy_entity(Entity& _e);

		// Processes all queued things
		void flush();


	private:
		std::vector<std::unique_ptr<AbstractComponentSystem>> data_{};
		EntityFactory factory_{};

		std::vector<Entity> dead_entities_{};

	};


};


namespace PROJECT_NAMESPACE
{
	template <typename T>
	struct component_system_type;

	template <typename T>
	concept cx_component = requires () { typename component_system_type<T>::type; };

	template <cx_component T>
	using component_system_type_t = typename component_system_type<T>::type;

};
