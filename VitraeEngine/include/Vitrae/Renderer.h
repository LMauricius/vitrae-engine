#pragma once

#include "Vitrae/Assets/Mesh.h"
#include "Vitrae/Assets/Texture.h"
#include "Vitrae/Util/Types.h"

#include "dynasma/managers/abstract.hpp"

namespace Vitrae
{
    class Renderer
    {
    public:
        virtual ~Renderer() = 0;

        virtual void setup() = 0;
        virtual void free() = 0;
        virtual void render() = 0;

        virtual Unique<MeshKeeper> newMeshManager() = 0;
        virtual Unique<TextureManager> newTextureManager() = 0;
    };

}