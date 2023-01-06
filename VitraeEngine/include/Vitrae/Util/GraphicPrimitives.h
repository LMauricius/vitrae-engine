#pragma once

#include "glm/glm.hpp"

namespace Vitrae
{

    struct Triangle
    {
        unsigned int ind[3];
    };
    struct Vertex
    {
        glm::vec3 pos, col, normal;
        glm::vec2 uv;
    };

}