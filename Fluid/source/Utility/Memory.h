#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace PROJECT_NAMESPACE
{

	template <typename T, typename... Ts> requires std::is_constructible_v<T, Ts&&...>
	static std::unique_ptr<T> make_unique(Ts&&... _ts)
	{
		return std::unique_ptr<T>{ new T{ std::forward<Ts>(_ts)... } };
	};

	template <typename T, typename... Ts> requires std::is_constructible_v<T, Ts&&...>
	static std::shared_ptr<T> make_shared(Ts&&... _ts)
	{
		return std::shared_ptr<T>{ new T{ std::forward<Ts>(_ts)... } };
	};


};
