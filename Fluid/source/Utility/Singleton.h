#pragma once

#include <optional>
#include <concepts>
#include <utility>

namespace PROJECT_NAMESPACE
{
	namespace impl
	{
		template <typename T, typename Tag>
		extern inline std::optional<T> SINGLETON_VARIABLE{ std::nullopt };
		template <typename T>
		static void default_construction(std::optional<T>& _opt) { _opt.emplace(); };
	};

	template <typename T, typename Tag = void, auto ConstructionFunction = impl::default_construction<T>>
	requires std::invocable<decltype(ConstructionFunction), std::optional<T>&>
		static T& get_singleton()
	{
		// Return optional's value, optionally constructing it if not yet initialized
		auto& _opt = impl::SINGLETON_VARIABLE<T, Tag>;
		if (_opt.has_value()) [[likely]]
		{
			return *_opt;
		}
		else
		{
			// Run emplace using tuple recieved from construction function and return built object
			std::invoke(ConstructionFunction, _opt);
			if (!_opt.has_value()) [[unlikely]]
			{
				// Fatal error should happen here
				std::terminate();
			};
			return *_opt;
		};
	};
};
