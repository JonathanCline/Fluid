#include "ECS.h"

namespace PROJECT_NAMESPACE
{

	template <typename T>
	static auto make_upointer_eq(T* _raw)
	{
		return [_raw](const std::unique_ptr<T>& v) -> bool
		{
			return v.get() == _raw;
		};
	};

	AbstractComponentSystem* EntityComponentSystem::insert_impl(std::unique_ptr<AbstractComponentSystem> _system)
	{
		auto _out = _system.get();
		this->data_.push_back(std::move(_system));
		return _out;
	};
	size_t EntityComponentSystem::erase(AbstractComponentSystem* _system)
	{
		return std::erase_if(this->data_, make_upointer_eq(_system));
	};

	void EntityComponentSystem::update()
	{
		for (auto& s : this->data_)
		{
			s->update();
		};
	};



	Entity EntityComponentSystem::new_entity()
	{
		auto& _fct = this->factory();
		return _fct.new_entity();
	};
	void EntityComponentSystem::destroy_entity(Entity& _e)
	{
		if (_e != null_entity)
		{
			this->dead_entities_.push_back(_e);
			auto& _fct = this->factory();
			_fct.recycle_entity(_e);
			_e = null_entity;
		};
	};


	void EntityComponentSystem::flush()
	{
		auto& _evec = this->dead_entities_;
		for (auto& s : this->data_)
		{
			s->erase_range(_evec);
		};
		_evec.clear();
	};



};