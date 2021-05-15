#pragma once

#include <SAELib_KeyVector.h>
#include <SAELib_Decorator.h>

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

	template <typename ComponentT>
	class ComponentSystem : public AbstractComponentSystem
	{
	protected:
		auto begin() noexcept { return this->data_.begin(); };
		auto cbegin() const noexcept { return this->data_.begin(); };
		auto begin() const noexcept { return this->cbegin(); };

		auto end() noexcept { return this->data_.end(); };
		auto cend() const noexcept { return this->data_.end(); };
		auto end() const noexcept { return this->cend(); };

	public:

		using component_type = ComponentT;

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

	protected:
		virtual component_type make_component(Entity _e) = 0;

	public:


		void insert(Entity _e) override
		{
			this->data_.insert({ _e, this->make_component(_e) });
		};
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
