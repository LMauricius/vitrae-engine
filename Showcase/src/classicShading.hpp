#include <iostream>

#include "shadingOverrider.hpp"

using namespace Vitrae;

struct ClassicShading : ShadingOverrider
{
    ClassicShading(ComponentRoot &root);
};