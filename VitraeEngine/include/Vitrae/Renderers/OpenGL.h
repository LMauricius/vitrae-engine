#pragma once

#include "Vitrae/Renderer.h"

namespace Vitrae
{
    class Mesh;
    class Material;
    
    class OpenGLRenderer
    {
    public:
        OpenGLRenderer();

        void setup();
        void free();
        void render();
        
        Unique<ResourceManager<Mesh>> newMeshManager();
        Unique<ResourceManager<Material>> newMaterialManager();
    };

}