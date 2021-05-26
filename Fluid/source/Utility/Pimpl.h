#pragma once

#include "Guard.h"

namespace PROJECT_NAMESPACE
{
	template <typename T, auto Destructor>
	class pimpl_ptr : public guard<T*, guard_traits<T*,
		Destructor,
		impl::default_guarded_value_make_null<T*>,
		impl::default_guarded_value_is_null<T*>>>
	{
	private:
		using parent_type = guard<T*, guard_traits<T*,
			Destructor,
			impl::default_guarded_value_make_null<T*>,
			impl::default_guarded_value_is_null<T*>>>;
	public:
		constexpr T* operator->() const noexcept { return this->get(); };
		constexpr explicit pimpl_ptr(T* _ptr) :
			parent_type{ _ptr }
		{};
	};

	template <typename T, auto Destructor>
	class ipimpl_ptr : public pimpl_ptr<T, Destructor>
	{
	public:
		constexpr operator T*& () noexcept { return this->get(); };
		constexpr operator T* const& () const noexcept { return this->get(); };
		using pimpl_ptr<T, Destructor>::pimpl_ptr;
	};

};
