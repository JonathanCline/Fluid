#include "Source.h"

#include <cassert>
#include <fstream>

namespace PROJECT_NAMESPACE
{
	namespace fs = std::filesystem;

	Source_File::data_type Source_File::read() const
	{
		const auto& _path = this->path();

		assert(fs::exists(_path));
		std::ifstream _fstr{ _path };
		assert(_fstr.is_open());

		char _rbuff[512]{ 0 };
		std::ranges::fill(_rbuff, 0);

		data_type _out{};
		while (!_fstr.eof())
		{
			_fstr.read(_rbuff, sizeof(_rbuff));
			const auto _count = _fstr.gcount();

			const auto _oldSize = _out.size();
			_out.resize(_out.size() + _count);
			std::copy(_rbuff, _rbuff + _count, reinterpret_cast<char*>(_out.data() + _oldSize));
		};

		return _out;
	};
	bool Source_File::has_changes() const
	{
		const auto _time  = fs::last_write_time(this->path());
		const auto _changed = (_time != this->last_change_time_);
		if (_changed) [[unlikely]]
		{
			this->last_change_time_ = _time;
		};
		return _changed;
	};





	Source_File::Source_File(const std::filesystem::path& _path) :
		path_{ _path }, last_change_time_{ fs::last_write_time(_path) }
	{};

};
