#pragma once

#include "glm/gtc/quaternion.hpp"

namespace Vitrae
{
    struct SimpleTransformation
    {
        glm::vec3 pos;
        glm::quat rot;
        glm::vec3 scale;
    };
}