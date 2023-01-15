#pragma once

#include "Vitrae/ResourceManager.h"

#include <variant>
#include "glm/glm.hpp"

namespace Vitrae
{
    class Texture;
    
    using ShaderProperty = std::variant<
        glm::vec1, glm::vec2, glm::vec3, glm::vec4,
        resource_ptr<Texture>
    >;
}