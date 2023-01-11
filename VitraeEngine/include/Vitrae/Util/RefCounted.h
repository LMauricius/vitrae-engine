#pragma once

#include <cstdlib>

namespace Vitrae
{
	struct RefCounted
	{
		std::size_t count;
	};
}