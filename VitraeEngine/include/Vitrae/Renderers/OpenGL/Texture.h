#pragma once

#include "ResourceManager.h"

#include <filesystem>

namespace Vitrae
{
    class ResourceRoot;

    /**
     * A Texture is a single image-like resource
    */
    class OpenGLTexture
    {
    public:
        struct SetupParams {};
        struct LoadParams {
            std::filesystem::path filepath;
            ResourceRoot &resRoot;
        };
        
        OpenGLTexture();
        ~OpenGLTexture();

        void load(const LoadParams &params, OpenGLRenderer & rend);
    protected:
        
    };
    
}