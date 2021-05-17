#pragma once

#include "ECS/ECS.h"

#include "Utility/Type.h"

#include <string>
#include <variant>
#include <memory>
#include <utility>

namespace fluid
{
	using Userdata = std::shared_ptr<std::byte>;

	class Field;

	class Form
	{
	public:

	};

	class Field
	{
	public:
		using typelist = std::tuple<null_t, bool, int, float, std::string, Userdata, std::shared_ptr<Form>>;
	private:
		using variant_type = sae::tuple_to_variant_t<typelist>;
	public:
		template <typename T>
		T& get() { return std::get<T>(this->vt_); };
		template <typename T>
		const T& get() const { return std::get<T>(this->vt_); };



	private:
		variant_type vt_{ null };
	};

	class Form_ComponentSysstem : public ComponentSystem<Form>
	{
	public:
		void update() override
		{

		};
	};
	
};
