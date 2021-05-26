#pragma once

/*
	Contains various metaprogramming utilities, mostly copied from SAELib
*/

#include <type_traits>
#include <concepts>
#include <variant>

namespace PROJECT_NAMESPACE
{
	struct null_t {};
	constexpr static null_t null{};

	template <typename T, typename... Ts>
	struct is_any_of : public std::bool_constant<(std::is_same_v<T, Ts> || ...)>
	{};
	template <typename T, typename... Ts>
	constexpr static auto is_any_of_v = is_any_of<T, Ts...>::value;

	template <typename T, typename... Ts>
	concept any_of = is_any_of_v<T, Ts...>;

	template <typename T, typename Tup>
	struct is_type_in : std::false_type {};
	template <typename T, template<typename... Ts> typename Tup, typename... Ts>
	requires any_of<T, Ts...>
	struct is_type_in<T, Tup<Ts...>> : std::true_type {};

	template <typename T, typename Tup>
	constexpr static auto is_type_in_v = is_type_in<T, Tup>::value;

	template <typename T, typename Tup>
	concept type_in = is_type_in_v<T, Tup>;



	using ts = std::tuple<int, bool, bool>;
	static_assert(is_type_in_v<int, ts> == true, "error");
	static_assert(is_type_in_v<char, ts> == false, "error");
	static_assert(is_type_in_v <int, bool> == false, "error");




	template <typename T>
	struct is_template_type : public std::false_type {};
	template <template <typename... Ts> typename T, typename... Ts>
	struct is_template_type<T<Ts...>> : public std::true_type {};

	template <typename T>
	constexpr static auto is_template_type_v = is_template_type<T>::value;

	template <typename T>
	concept template_type = is_template_type_v<T>;

	template <typename T>
	struct make_variant_from;

	template <template <typename... Ts> typename Tup, typename... Ts>
	struct make_variant_from<Tup<Ts...>>
	{
		using type = std::variant<Ts...>;
	};
	template <template_type T>
	using make_variant_from_t = typename make_variant_from<T>::type;




	namespace impl
	{
		template <typename T, typename U, size_t Idx>
		consteval static size_t tuple_index_finder_idx() noexcept
		{
			return ((std::is_same_v<T, U>) ? Idx : 0);
		};

		template <typename T, template <typename... Ts> typename Tup, typename... Ts, size_t... Idxs> requires is_any_of_v<T, Ts...>
			consteval static auto tuple_index_finder(Tup<Ts...>* _tup, std::index_sequence<Idxs...> _isq) noexcept
			{
				return (tuple_index_finder_idx<T, Ts, Idxs>() + ...);
			};
	};

	template <typename T, typename Tup>
	struct tuple_element_index;

	template <typename T, template <typename... Ts> typename Tup, typename... Ts> requires ((((std::is_same_v<T, Ts>) ? 1 : 0) + ...) == 1)
		struct tuple_element_index<T, Tup<Ts...>>
	{
		using type = size_t;
		constexpr static type value = impl::tuple_index_finder<T>((Tup<Ts...>*)(nullptr), std::make_index_sequence<sizeof...(Ts)>{});
	};

	template <typename T, typename Tup>
	requires requires () { typename tuple_element_index<T, Tup>::type; }
	constexpr static auto tuple_element_index_v = tuple_element_index<T, Tup>::value;





	namespace impl
	{
		template <auto Op, typename... T> requires std::invocable<decltype(Op), const T&...>
		consteval static bool passes_lamda_concept()
		{
			return true;
		};
		template <auto Op, typename... T>
		consteval static bool passes_lamda_concept()
		{
			return false;
		};
	};

	template <auto ConceptLamda, typename... T>
	struct passes_concept : std::bool_constant<impl::passes_lamda_concept<ConceptLamda, T...>()> {};

	template <auto ConceptLamda, typename... T>
	constexpr static bool passes_concept_v = passes_concept<ConceptLamda, T...>::value;

#define CONCEPTWRAP(cxname) [](auto... f) requires cxname <decltype(f)...>{}

};
