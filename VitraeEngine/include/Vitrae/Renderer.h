#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/ResourceManager.h"

namespace Vitrae
{
    class Mesh;
    class Texture;

    class Renderer
    {
    public:
        virtual ~Renderer() = 0;

        virtual void setup() = 0;
        virtual void free() = 0;
        virtual void render() = 0;
        
        virtual Unique<ResourceManager<Mesh>> newMeshManager() = 0;
        virtual Unique<ResourceManager<Texture>> newTextureManager() = 0;
    };

}