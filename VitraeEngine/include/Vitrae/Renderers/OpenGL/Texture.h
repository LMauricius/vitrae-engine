#pragma once

#include "Vitrae/Assets/Texture.h"
#include "glad/glad.h"

#include <filesystem>

namespace Vitrae
{
    class ComponentRoot;
    class OpenGLRenderer;

    /**
     * A Texture is a single image-like resource
    */
    class OpenGLTexture: public Texture
    {
    public:
        OpenGLTexture(const FileLoadParams &params);
        ~OpenGLTexture();

        void loadToGPU(OpenGLRenderer &rend);
        void unloadFromGPU(OpenGLRenderer &rend);

        std::size_t memory_cost() const override;

        GLuint mGLTexture;

    protected:
	    int mWidth, mHeight, mGLChannelFormat;
        unsigned char *mData;

        bool m_sentToGPU;
    };
    
}