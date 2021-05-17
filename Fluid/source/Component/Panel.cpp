#include "Panel.h"

namespace PROJECT_NAMESPACE
{
	void Panel_ComponentSystem::push_event(const PanelEvent& _event)
	{
		this->events_.push(_event);
	};




	void Panel_ComponentSystem::update()
	{
		auto& _events = this->events_;
		while (!_events.empty())
		{
			const auto _event = std::move(_events.front());
			const auto _eventType = _event.type();
		
			switch (_eventType)
			{
			case _event.evResize:
			{
				const auto& _ev = _event.get<_event.evResize>();
				for (auto& _panel : *this)
				{
					auto& [_entity, _pval] = _panel;
					_pval.resize(_entity, _ev.width, _ev.height);

			
				};
			};
				break;
			case _event.evMouse:
			{
				const auto& _ev = _event.get<_event.evMouse>();
				for (auto& _panel : *this)
				{
					auto& [_entity, _pval] = _panel;
					_pval.mouse(_entity, _ev.button, _ev.action, _ev.mods);
				};
			};
				break;
			default: // terminate on unrecognized event occurs (debug mode)
#ifndef NDEBUG
				std::terminate();
#endif
				break;
			};
			// useless comments are not useless, they make the code prettier

			_events.pop();
		};

	};

};