#pragma once

#include "Vitrae/Resources/Texture.h"
#include "Vitrae/ResourceManager.h"
#include "glad/glad.h"

#include <filesystem>

namespace Vitrae
{
    class ResourceRoot;
    class OpenGLRenderer;

    /**
     * A Texture is a single image-like resource
    */
    class OpenGLTexture: public Texture
    {
    public:
        OpenGLTexture();
        ~OpenGLTexture();

        void load(const LoadParams &params, OpenGLRenderer &rend);
        void unload();
        void loadToGPU(OpenGLRenderer &rend);
        void unloadFromGPU(OpenGLRenderer &rend);

        GLuint mGLTexture;

    protected:
	    int mWidth, mHeight, mGLChannelFormat;
        unsigned char *mData;
    };
    
}