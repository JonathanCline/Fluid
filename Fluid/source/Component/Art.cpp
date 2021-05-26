#include "Art.h"
#include "Art/AbstractArt.h"

#include "Utility/Type.h"

#include <SAELib_Type.h>
#include <tuple>
#include <concepts>

namespace fluid
{
	namespace impl
	{
		void delete_art(AbstractArt*& _art)
		{
			delete _art;
			_art = nullptr;
		};
	};

	void Art::draw()
	{
		if (this->art)
		{
			this->art->draw();
		};
	};

	void Art_ComponentSystem::update()
	{
		for (auto& e : *this)
		{
			auto& [_entity, _component] = e;
			_component.draw();
		};
	};

};
