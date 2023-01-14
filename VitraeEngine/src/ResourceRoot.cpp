#include "Vitrae/ResourceRoot.h"
#include "Vitrae/ResourceManager.h"

#include <iostream>

namespace Vitrae
{
	ResourceRoot::ResourceRoot():
		mInfoStream(&std::cout),
		mWarningStream(&std::cout),
		mErrStream(&std::cerr)
	{

	}
	
	ResourceRoot::~ResourceRoot()
	{

	}
}