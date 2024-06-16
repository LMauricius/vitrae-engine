#include "Vitrae/Renderers/OpenGL/Texture.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Renderers/OpenGL.hpp"

#include "stb/stb_image.h"

#include <map>
#include <vector>

namespace Vitrae
{
OpenGLTexture::OpenGLTexture(WrappingType horWrap, WrappingType verWrap, FilterType minFilter,
                             FilterType magFilter, bool useMipMaps, glm::vec4 borderColor)
    : m_sentToGPU(false)
{
    switch (horWrap) {
    case WrappingType::BORDER_COLOR:
        mGLWrapS = GL_CLAMP_TO_BORDER;
        break;
    case WrappingType::CLAMP:
        mGLWrapS = GL_CLAMP_TO_EDGE;
        break;
    case WrappingType::REPEAT:
        mGLWrapS = GL_REPEAT;
        break;
    case WrappingType::MIRROR:
        mGLWrapS = GL_MIRRORED_REPEAT;
        break;
    }
    switch (verWrap) {
    case WrappingType::BORDER_COLOR:
        mGLWrapT = GL_CLAMP_TO_BORDER;
        break;
    case WrappingType::CLAMP:
        mGLWrapT = GL_CLAMP_TO_EDGE;
        break;
    case WrappingType::REPEAT:
        mGLWrapT = GL_REPEAT;
        break;
    case WrappingType::MIRROR:
        mGLWrapT = GL_MIRRORED_REPEAT;
        break;
    }
    switch (magFilter) {
    case FilterType::LINEAR:
        mGLMagFilter = GL_LINEAR;
        break;
    case FilterType::NEAREST:
        mGLMagFilter = GL_NEAREST;
        break;
    }
    if (useMipMaps) {
        switch (minFilter) {
        case FilterType::LINEAR:
            mGLMinFilter = GL_LINEAR_MIPMAP_LINEAR;
            break;
        case FilterType::NEAREST:
            mGLMinFilter = GL_NEAREST_MIPMAP_NEAREST;
            break;
        }
    } else {
        switch (minFilter) {
        case FilterType::LINEAR:
            mGLMinFilter = GL_LINEAR;
            break;
        case FilterType::NEAREST:
            mGLMinFilter = GL_NEAREST;
            break;
        }
    }
    mUseMipMaps = useMipMaps;
    mBorderColor = borderColor;
}

OpenGLTexture::OpenGLTexture(const FileLoadParams &params)
    : OpenGLTexture(params.horWrap, params.verWrap, params.minFilter, params.magFilter,
                    params.useMipMaps, {0.0f, 0.0f, 0.0f, 0.0f})
{
    int stbChannelFormat;
    unsigned char *data =
        stbi_load(params.filepath.c_str(), &mWidth, &mHeight, &stbChannelFormat, STBI_rgb_alpha);

    mUseSwizzle = false;
    switch (stbChannelFormat) {
    case STBI_grey:
        mGLChannelFormat = GL_RED;
        mSwizzle = {GL_RED, GL_RED, GL_RED, GL_ONE};
        mUseSwizzle = true;
        break;
    case STBI_grey_alpha:
        params.root.getWarningStream()
            << "Texture load cannot convert from gray_alpha format; red_green used!" << std::endl;
        mGLChannelFormat = GL_RG;
        mSwizzle = {GL_RED, GL_RED, GL_RED, GL_GREEN};
        mUseSwizzle = true;
        break;
    case STBI_rgb:
        mGLChannelFormat = GL_RGB;
        break;
    case STBI_rgb_alpha:
        mGLChannelFormat = GL_RGBA;
        break;
    }

    if (stbi_failure_reason()) {
        params.root.getErrStream() << "Texture load from '" << params.filepath
                                   << "' failed: " << stbi_failure_reason() << std::endl;
        loadToGPU(nullptr);

        stbi_image_free(data);
    } else {
        params.root.getInfoStream() << "Texture '" << params.filepath << "' loaded." << std::endl;
        loadToGPU(data);
    }
}

OpenGLTexture::OpenGLTexture(const EmptyParams &params)
    : OpenGLTexture(params.horWrap, params.verWrap, params.minFilter, params.magFilter,
                    params.useMipMaps, params.borderColor)
{
    mUseSwizzle = false;
    switch (params.channelType) {
    case ChannelType::GRAYSCALE:
        mGLChannelFormat = GL_RED;
        mSwizzle = {GL_RED, GL_RED, GL_RED, GL_ONE};
        mUseSwizzle = true;
        break;
    case ChannelType::GRAYSCALE_ALPHA:
        mGLChannelFormat = GL_RG;
        mSwizzle = {GL_RED, GL_RED, GL_RED, GL_GREEN};
        mUseSwizzle = true;
        break;
    case ChannelType::RGB:
        mGLChannelFormat = GL_RGB;
        break;
    case ChannelType::RGBA:
        mGLChannelFormat = GL_RGBA;
        break;
    case ChannelType::DEPTH:
        mGLChannelFormat = GL_DEPTH_COMPONENT;
        break;
    }

    mWidth = params.size.x;
    mHeight = params.size.y;

    loadToGPU(nullptr);
}

OpenGLTexture::~OpenGLTexture()
{
    unloadFromGPU();
}

std::size_t OpenGLTexture::memory_cost() const
{
    return mWidth * mHeight * 3; /// TODO: properly calculate num channels
}

void OpenGLTexture::loadToGPU(const unsigned char *data)
{
    if (!m_sentToGPU) {
        m_sentToGPU = true;
        glGenTextures(1, &glTextureId);
        glBindTexture(GL_TEXTURE_2D, glTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, mGLChannelFormat, mWidth, mHeight, 0, mGLChannelFormat,
                     GL_UNSIGNED_BYTE, data);

        if (mUseSwizzle) {
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, mSwizzleArr);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mGLMagFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mGLMinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mGLWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mGLWrapT);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &mBorderColor[0]);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void OpenGLTexture::unloadFromGPU()
{
    if (m_sentToGPU) {
        m_sentToGPU = false;
        glDeleteTextures(1, &glTextureId);
    }
}

} // namespace Vitrae