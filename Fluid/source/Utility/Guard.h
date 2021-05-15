#pragma once

#include <type_traits>
#include <utility>

namespace PROJECT_NAMESPACE
{
	template <typename T,
		void(*destructor_v)(T&),
		void(*make_null_v)(T&),
		bool(*is_null_v)(const T&)>
		struct guard_traits
	{
		constexpr static auto destructor = destructor_v;
		constexpr static auto make_null = make_null_v;
		constexpr static auto is_null = is_null_v;
	};

	namespace impl
	{
		template <typename T> requires std::is_default_constructible_v<T>
			constexpr static void default_guarded_value_make_null(T& _val)
			{
				_val = NULL;
			};
			template <typename T> requires requires (const T& a) { (bool)a; }
			constexpr static bool default_guarded_value_is_null(const T& _val)
			{
				return (bool)_val;
			};

			template <typename T>
			using default_guard_traits = guard_traits
				<
					T,
					default_guarded_value_make_null<T>,
					default_guarded_value_make_null<T>,
					default_guarded_value_is_null<T>
				>;
	};

	template <typename T, typename TraitsT = impl::default_guard_traits<T>>
	class guard
	{
	public:
		using value_type = T;
		using traits = impl::default_guard_traits<value_type>;

		constexpr value_type& get() noexcept { return this->value_; };
		constexpr const value_type& get() const noexcept { return this->value_; };

		constexpr value_type& operator*() noexcept { return this->get(); };
		constexpr const value_type& operator*() const noexcept { return this->get(); };

		constexpr bool good() const noexcept { return (bool)this->get(); };
		constexpr explicit operator bool() const noexcept { return this->good(); };

		constexpr void release() noexcept
		{
			std::invoke(traits::make_null, this->get());
		};
		constexpr auto extract() noexcept
		{
			auto _out = this->get();
			this->release();
			return _out;
		};
		constexpr void reset() noexcept
		{
			if (this->good())
			{
				std::invoke(traits::destructor, this->get());
			};
		};

		constexpr explicit guard() :
			value_{}
		{};
		constexpr explicit guard(value_type _value) :
			value_{ std::move(_value) }
		{};

		guard(const guard&) = delete;
		guard& operator=(const guard&) = delete;

		constexpr guard(guard&& _other) noexcept :
			value_{ _other.extract() }
		{};
		constexpr guard& operator=(guard&& _other) noexcept
		{
			this->reset();
			this->value_ = std::move(_other.extract());
			return *this;
		};

		~guard()
		{
			this->reset();
		};

	private:
		value_type value_;
	};

	template <typename T, auto Destructor>
	using scope_guard_traits = guard_traits<T,
		Destructor,
		impl::default_guarded_value_make_null<T>,
		impl::default_guarded_value_is_null<T>>;



	template <auto Destructor>
	struct scope_guard
	{
	public:
		void reset()
		{
			if (this->tempo_)
			{
				std::invoke(Destructor);
			};
		};
		scope_guard() = default;
		~scope_guard()
		{
			this->reset();
		};
	private:
		guard<bool> tempo_{ true };
	};


};