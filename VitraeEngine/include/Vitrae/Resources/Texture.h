#pragma once

#include "Vitrae/ResourceManager.h"

#include <filesystem>

namespace Vitrae
{
    class ResourceRoot;

    /**
     * A Texture is a single image-like resource
    */
    class Texture
    {
    public:
        struct SetupParams {};
        struct LoadParams {
            std::filesystem::path filepath;
            ResourceRoot &resRoot;
        };
        
        virtual ~Texture() = 0;
    };
    
}