#pragma once

#include "dynasma/core_concepts.hpp"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"

#include "glm/glm.hpp"

#include <filesystem>

namespace Vitrae
{
class ComponentRoot;

/**
 * A Texture is a single image-like resource
 */
class Texture : public dynasma::PolymorphicBase
{
  public:
    enum class ChannelType {
        GRAYSCALE,
        GRAYSCALE_ALPHA,
        RGB,
        RGBA,
        DEPTH
    };
    enum class WrappingType {
        BORDER_COLOR,
        CLAMP,
        REPEAT,
        MIRROR
    };
    enum class FilterType {
        NEAREST,
        LINEAR
    };

    struct FileLoadParams
    {
        ComponentRoot &root;
        std::filesystem::path filepath;
        WrappingType horWrap = WrappingType::REPEAT;
        WrappingType verWrap = WrappingType::REPEAT;
        FilterType minFilter = FilterType::LINEAR;
        FilterType magFilter = FilterType::LINEAR;
        bool useMipMaps = true;
    };
    struct EmptyParams
    {
        ComponentRoot &root;
        glm::vec2 size;
        ChannelType channelType = ChannelType::RGB;
        WrappingType horWrap = WrappingType::REPEAT;
        WrappingType verWrap = WrappingType::REPEAT;
        FilterType minFilter = FilterType::LINEAR;
        FilterType magFilter = FilterType::LINEAR;
        bool useMipMaps = true;
        glm::vec4 borderColor = {0.0f, 0.0f, 0.0f, 0.0f};
    };
    struct PureColorParams
    {
        ComponentRoot &root;
        glm::vec4 color;
    };

    virtual ~Texture() = default;

    virtual std::size_t memory_cost() const = 0;

    inline glm::vec2 getSize() const { return glm::vec2(mWidth, mHeight); }

  protected:
    int mWidth, mHeight;
};

struct TextureSeed
{
    using Asset = Texture;

    inline std::size_t load_cost() const { return 1; }

    std::variant<Texture::FileLoadParams, Texture::EmptyParams, Texture::PureColorParams> kernel;
};

using TextureManager = dynasma::AbstractManager<TextureSeed>;
// using TextureKeeper = dynasma::AbstractKeeper<ImmediateTextureSeed>;
} // namespace Vitrae