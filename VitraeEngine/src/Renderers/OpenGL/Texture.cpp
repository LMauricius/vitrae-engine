#include "Vitrae/Renderers/OpenGL/Texture.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"

#include "stb/stb_image.h"

#include <map>
#include <vector>

namespace Vitrae
{
OpenGLTexture::OpenGLTexture(const FileLoadParams &params) : mData(nullptr), m_sentToGPU(false)
{
    int stbChannelFormat;
    mData =
        stbi_load(params.filepath.c_str(), &mWidth, &mHeight, &stbChannelFormat, STBI_rgb_alpha);

    switch (stbChannelFormat) {
    case STBI_grey:
        mGLChannelFormat = GL_RED;
        break;
    case STBI_grey_alpha:
        params.root.getWarningStream()
            << "Texture load cannot convert from gray_alpha format; red_green used!" << std::endl;
        mGLChannelFormat = GL_RG;
        break;
    case STBI_rgb:
        mGLChannelFormat = GL_RGB;
        break;
    case STBI_rgb_alpha:
        mGLChannelFormat = GL_RGBA;
        break;
    }

    params.root.getErrStream() << "Texture load from '" << params.filepath
                               << "' failed: " << stbi_failure_reason() << std::endl;
}

OpenGLTexture::~OpenGLTexture()
{
    if (mData) {
        stbi_image_free(mData);
    }
}

std::size_t OpenGLTexture::memory_cost() const
{
    return mWidth * mHeight * 3; /// TODO: properly calculate num channels
}

void OpenGLTexture::loadToGPU(OpenGLRenderer &rend)
{
    if (!m_sentToGPU) {
        m_sentToGPU = true;
        glGenTextures(1, &mGLTexture);
        glBindTexture(GL_TEXTURE_2D, mGLTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, mGLChannelFormat, mWidth, mHeight, 0, mGLChannelFormat,
                     GL_UNSIGNED_BYTE, mData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void OpenGLTexture::unloadFromGPU(OpenGLRenderer &rend)
{
    if (m_sentToGPU) {
        m_sentToGPU = false;
        glDeleteTextures(1, &mGLTexture);
    }
}

} // namespace Vitrae