#pragma once

#include <lua.hpp>

#include <cassert>
#include <utility>
#include <type_traits>
#include <concepts>

namespace PROJECT_NAMESPACE
{
	struct index_abs
	{
	public:
		auto& get() noexcept { return this->idx_; };
		const auto& get() const noexcept { return this->idx_; };

		operator auto& () noexcept { return this->get(); };
		operator const auto& () const noexcept { return this->get(); };

		constexpr explicit index_abs(lua_Integer _index) :
			idx_{ _index }
		{
			assert((_index > 0) || (_index < LUA_REGISTRYINDEX));
		};
		explicit index_abs(lua_State* _lua, lua_Integer _index) :
			idx_{ lua_absindex(_lua, _index) }
		{};

	private:
		lua_Integer idx_;
	};
	static index_abs abs(lua_State* _lua, int _idx)
	{
		return index_abs{ lua_absindex(_lua, _idx) };
	};



	struct nil_t {};
	constexpr static nil_t nil{};



	template <typename T>
	struct push_t;

	/*
	template <typename T>
	struct push_t
	{
		int operator()(lua_State* _lua, const T& _v)
		{
			// push code goes here
			// should return number of values pushed
		};
	};
	*/

	template <typename T>
	concept pushable = requires (lua_State * _lua, T a)
	{
		push_t<T>{}(_lua, a);
	};

	template <pushable... T>
	static int push(lua_State* _lua, const T&... _val)
	{
		return (push_t<T>{}(_lua, _val) + ...);
	};




	template <typename T>
	struct pull_t;

	template <typename T>
	concept pullable = requires (lua_State * _lua, index_abs _index)
	{
		{ pull_t<T>{}(_lua, _index) } -> std::same_as<T>;
	};

	template <pullable T>
	static T pull(lua_State* _lua, const index_abs _idx)
	{
		return pull_t<T>{}(_lua, _idx);
	};











	template <typename T> requires std::integral<T> && (sizeof(T) <= sizeof(lua_Integer))
		struct push_t<T>
	{
		int operator()(lua_State* _lua, T _val)
		{
			lua_pushinteger(_lua, (lua_Integer)_val);
			return 1;
		};
	};
	template <typename T>
	requires std::integral<T> && (sizeof(T) <= sizeof(lua_Integer))
		struct pull_t<T>
	{
		T operator()(lua_State* _lua, const index_abs _idx)
		{
			return lua_tointeger(_lua, _idx);
		};
	};

	template <>
	struct push_t<bool>
	{
		int operator()(lua_State* _lua, bool _val)
		{
			lua_pushboolean(_lua, _val);
			return 1;
		};
	};
	template <>
	struct pull_t<bool>
	{
		bool operator()(lua_State* _lua, const index_abs _idx)
		{
			return lua_toboolean(_lua, _idx);
		};
	};
	

	template <>
	struct push_t<const char*>
	{
		int operator()(lua_State* _lua, const char* _val)
		{
			lua_pushstring(_lua, _val);
			return 1;
		};
	};
	template <>
	struct pull_t<const char*>
	{
		const char* operator()(lua_State* _lua, const index_abs _idx)
		{
			return lua_tostring(_lua, _idx);
		};
	};



	template <>
	struct push_t<std::string>
	{
		int operator()(lua_State* _lua, const std::string& _val)
		{
			lua_pushstring(_lua, _val.c_str());
			return 1;
		};
	};
	template <>
	struct pull_t<std::string>
	{
		std::string operator()(lua_State* _lua, const index_abs _idx)
		{
			return std::string{ lua_tostring(_lua, _idx) };
		};
	};

	template <>
	struct push_t<float_t>
	{
		int operator()(lua_State* _lua, float_t _val)
		{
			lua_pushnumber(_lua, (lua_Number)_val);
			return 1;
		};
	};
	template <>
	struct pull_t<float_t>
	{
		auto operator()(lua_State* _lua, const index_abs _idx)
		{
			return (float_t)lua_tonumber(_lua, _idx);
		};
	};

	template <>
	struct push_t<double_t>
	{
		int operator()(lua_State* _lua, double_t _val)
		{
			lua_pushnumber(_lua, (lua_Number)_val);
			return 1;
		};
	};
	template <>
	struct pull_t<double_t>
	{
		auto operator()(lua_State* _lua, const index_abs _idx)
		{
			return (double_t)lua_tonumber(_lua, _idx);
		};
	};





	template <>
	struct push_t<nil_t>
	{
		int operator()(lua_State* _lua, nil_t)
		{
			lua_pushnil(_lua);
			return 1;
		};
	};

	template <>
	static int push(lua_State* _lua)
	{
		return push(_lua, nil);
	};



	static void pop(lua_State* _lua, size_t _count)
	{
		lua_pop(_lua, _count);
	};
	static void pop(lua_State* _lua)
	{
		pop(_lua, 1);
	};




	template <typename T>
	concept library = requires (lua_State * l)
	{
		{ T::openlib(l) } -> std::same_as<int>;
	}&& std::constructible_from<std::string, decltype(T::libname)>;

	template <typename T>
	struct is_library : std::false_type {};
	template <typename T>
	requires library<T>
		struct is_library<T> : std::true_type {};

	template <typename T>
	constexpr auto is_library_v = is_library<T>::value;


	template <>
	struct push_t<lua_CFunction>
	{
		int operator()(lua_State* _lua, const lua_CFunction& _f)
		{
			lua_pushcfunction(_lua, _f);
			return 1;
		};
	};

	// Helper for lua cfunction references, makes the push() interface a bit nicer
	template <>
	struct push_t<int(lua_State*)> : public push_t<lua_CFunction>
	{};



	template <library T>
	struct push_t<T>
	{
		int operator()(lua_State* _lua, const T& _lib)
		{
			return T::openlib(_lua);
		};
	};




	template <typename T>
	struct is_closure : std::false_type
	{

	};
	template <typename... Ts> requires ((pushable<Ts>&& pullable<Ts>) && ...)
		struct is_closure<int(*)(lua_State*, Ts...)> : std::true_type
	{
		constexpr static auto upvalues = sizeof...(Ts);
	};
	template <auto ClosureFunction> requires (is_closure<decltype(ClosureFunction)>::value)
		struct closure
	{
		template <typename... Ts, size_t... Idxs>
		static int cfunction_helper(lua_State* _lua, int(*_functionPtr)(lua_State*, Ts...), std::index_sequence<Idxs...>)
		{
			return std::invoke(_functionPtr, _lua, pull<Ts>(_lua, index_abs{ lua_upvalueindex((int)Idxs + 1) })...);
		};
		static int cfunction(lua_State* _lua)
		{
			return cfunction_helper(_lua, ClosureFunction, std::index_sequence<is_closure<decltype(ClosureFunction)>::upvalues>{});
		};

	};

	template <auto ClosureFunction, typename... Ts> requires (is_closure<decltype(ClosureFunction)>::upvalues == sizeof...(Ts))
		static int push_closure(lua_State* _lua, closure<ClosureFunction> _closure, Ts... _ups)
	{
		const auto n = push(_lua, _ups...);
		lua_pushcclosure(_lua, closure<ClosureFunction>::cfunction, n);
		return 1;
	};

};