#include "Vitrae/Renderers/OpenGL/Texture.h"
#include "Vitrae/Renderers/OpenGL.h"
#include "Vitrae/ComponentRoot.h"

#include "stb/stb_image.h"

#include <vector>
#include <map>

namespace Vitrae
{
    OpenGLTexture::OpenGLTexture():
        mData(nullptr)
    {
    }

    OpenGLTexture::~OpenGLTexture()
    {
		if (mData)
		{
			stbi_image_free(mData);
		}
    }

    void OpenGLTexture::load(const LoadParams &params, OpenGLRenderer & rend)
    {
        int stbChannelFormat;
        mData = stbi_load(
            params.filepath.c_str(), &mWidth, &mHeight, &stbChannelFormat, STBI_rgb_alpha
        );

        switch (stbChannelFormat) {
        case STBI_grey:
            mGLChannelFormat = GL_RED;
            break;
        case STBI_grey_alpha:
            params.resRoot.getWarningStream() << "Texture load cannot convert from gray_alpha format; red_green used!" << std::endl;
            mGLChannelFormat = GL_RG;
            break;
        case STBI_rgb:
            mGLChannelFormat = GL_RGB;
            break;
        case STBI_rgb_alpha:
            mGLChannelFormat = GL_RGBA;
            break;
        }

        params.resRoot.getErrStream() << "Texture load from '" << params.filepath << "' failed: " <<
            stbi_failure_reason() << std::endl;
    }

    void OpenGLTexture::unload()
    {
        stbi_image_free(mData);
        mData = nullptr;
    }

    void OpenGLTexture::loadToGPU(OpenGLRenderer &rend)
    {
		glGenTextures(1, &mGLTexture);
		glBindTexture(GL_TEXTURE_2D, mGLTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, mGLChannelFormat, mWidth, mHeight, 0, mGLChannelFormat, GL_UNSIGNED_BYTE, mData);
		glGenerateMipmap(GL_TEXTURE_2D);
    }

    void OpenGLTexture::unloadFromGPU(OpenGLRenderer &rend)
    {
		glDeleteTextures(1, &mGLTexture);
    }
    
}