#pragma once

#include "Vitrae/ResourceManager.h"

#include <variant>
#include "glm/glm.hpp"

namespace Vitrae
{
    class Texture;
    
    using VariantProperty = std::variant<
        int32_t,
        glm::vec1, glm::vec2, glm::vec3, glm::vec4,
        glm::mat2, glm::mat3, glm::mat4,
        resource_ptr<Texture>
    >;
    
    struct VariantPropertySpec
    {
        VariantProperty exampleValue;
    };
}