#pragma once

#include "Vitrae/Assets/Mesh.hpp"
#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/Types/Typedefs.hpp"

#include "dynasma/managers/abstract.hpp"

namespace Vitrae
{
class ComponentRoot;

class Renderer
{
  public:
    virtual ~Renderer() = default;

    virtual void setup(ComponentRoot &root) = 0;
    virtual void free() = 0;
    virtual void render() = 0;
};

} // namespace Vitrae