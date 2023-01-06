#pragma once

#include <cstdlib>

namespace Vitrae
{
	inline size_t getUniqueID()
	{
		static size_t ctr = 0;
		ctr++;
		return ctr;
	}

	template <class T>
	class ClassWithID
	{
	public:
		static size_t getClassID()
		{
			static size_t id = getUniqueID();
			return id;
		}
	};
}