#pragma once

#include <vector>
#include <utility>
#include <filesystem>
#include <string>
#include <memory>
#include <algorithm>
#include <ranges>

namespace PROJECT_NAMESPACE
{

	class Source
	{
	public:
		using data_type = std::vector<std::byte>;

		virtual bool has_changes() const = 0;
		virtual data_type read() const = 0;
		virtual ~Source() = default;
	};

	class Source_File : public Source
	{
	public:
		bool has_changes() const override;
		data_type read() const override;

		auto& path() noexcept
		{
			return this->path_;
		};
		const auto& path() const noexcept
		{
			return this->path_;
		};

		Source_File(const std::filesystem::path& _path);

	private:
		std::filesystem::path path_;
		mutable std::filesystem::file_time_type last_change_time_;
	};

	template <std::ranges::range ContainerT>
	requires (std::is_trivial_v<std::ranges::range_value_t<ContainerT>> && sizeof(std::ranges::range_value_t<ContainerT>) == 1)
	class Source_Data : public Source
	{
	public:
		using container_type = ContainerT;
		using value_type = typename container_type::value_type;

		bool has_changes() const override { return false; };
		data_type read() const override
		{
			if constexpr (std::is_same_v<container_type, data_type>)
			{
				return this->data_;
			}
			else
			{
				std::vector<std::byte> _out{};
				_out.resize(this->data_.size() * sizeof(value_type));
				auto _dest = reinterpret_cast<value_type*>(_out.data());
				std::ranges::copy(this->data_, _dest);
				return _out;
			};
		};

		Source_Data() = default;
		Source_Data(const container_type& _data) :
			data_{ _data }
		{};

	private:
		container_type data_;
	};

};

