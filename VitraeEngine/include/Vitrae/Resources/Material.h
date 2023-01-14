#pragma once

#include "assimp/material.h"

namespace Vitrae
{

	class Material
	{
	public:
        struct SetupParams
        {
            const aiMaterial& extMaterial;
            ResourceRoot &resRoot;
        };
        struct LoadParams {};

		Material();
	};
}