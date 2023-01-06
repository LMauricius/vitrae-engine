#pragma once

#include "assimp/types.h"
#include "glm/glm.hpp"

namespace Vitrae
{

    template <class T>
    struct aiTypeCvt
	{
		using glmType = T;
	};

	template <>
    struct aiTypeCvt<aiVector2D>
	{
		using glmType = glm::vec2;
	};

	template <>
    struct aiTypeCvt<aiVector3D>
	{
		using glmType = glm::vec3;
	};

	template <>
    struct aiTypeCvt<aiColor3D>
	{
		using glmType = glm::vec3;
	};

	template <>
    struct aiTypeCvt<aiColor4D>
	{
		using glmType = glm::vec4;
	};
}