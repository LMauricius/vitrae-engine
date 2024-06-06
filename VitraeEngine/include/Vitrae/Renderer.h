#pragma once

#include "Vitrae/Assets/Mesh.h"
#include "Vitrae/Assets/Texture.h"
#include "Vitrae/Types/Typedefs.h"

#include "dynasma/managers/abstract.hpp"

namespace Vitrae
{
class ComponentRoot;

class Renderer
{
  public:
    virtual ~Renderer() = 0;

    virtual void setup(ComponentRoot &root) = 0;
    virtual void free() = 0;
    virtual void render() = 0;
};

} // namespace Vitrae