#pragma once

namespace fluid
{
	class AbstractArt
	{
	public:
		virtual void draw() = 0;
		virtual ~AbstractArt() = default;
	};
};
