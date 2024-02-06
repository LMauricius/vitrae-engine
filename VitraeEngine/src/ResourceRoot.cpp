#include "Vitrae/ComponentRoot.h"

#include <iostream>

namespace Vitrae
{
	ComponentRoot::ComponentRoot() : mInfoStream(&std::cout),
									 mWarningStream(&std::cout),
									 mErrStream(&std::cerr)
	{

	}

	ComponentRoot::~ComponentRoot()
	{

	}
}