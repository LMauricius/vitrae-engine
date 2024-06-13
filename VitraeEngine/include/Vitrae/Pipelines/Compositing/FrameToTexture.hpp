#pragma once

#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Compositing/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include "glm/glm.hpp"

#include <variant>

namespace Vitrae
{

class ComposeFrameToTexture : public ComposeTask
{
  public:
    struct SetupParams
    {
        ComponentRoot &root;
        String frameInputPropertyName;
        String colorTextureOutputPropertyName;
        String depthTextureOutputPropertyName;
        glm::vec2 size;
        Texture::ChannelType channelType = Texture::ChannelType::RGB;
        Texture::WrappingType horWrap = Texture::WrappingType::REPEAT;
        Texture::WrappingType verWrap = Texture::WrappingType::REPEAT;
        Texture::FilterType minFilter = Texture::FilterType::LINEAR;
        Texture::FilterType magFilter = Texture::FilterType::LINEAR;
        bool useMipMaps = true;
        glm::vec4 borderColor = {0.0f, 0.0f, 0.0f, 0.0f};
    };

    ComposeFrameToTexture(const SetupParams &params);
    ~ComposeFrameToTexture() = default;

    void run(RenderRunContext args) const override;
    void prepareRequiredLocalAssets(
        std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
        std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const override;

  protected:
    ComponentRoot &m_root;
    StringId m_frameInputNameId, m_colorTextureOutputNameId, m_depthTextureOutputNameId;
    glm::vec2 m_size;
    Texture::ChannelType m_channelType;
    Texture::WrappingType m_horWrap;
    Texture::WrappingType m_verWrap;
    Texture::FilterType m_minFilter;
    Texture::FilterType m_magFilter;
    bool m_useMipMaps;
    glm::vec4 m_borderColor;
};

struct ComposeFrameToTextureKeeperSeed
{
    using Asset = ComposeFrameToTexture;
    std::variant<ComposeFrameToTexture::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ComposeFrameToTextureKeeper = dynasma::AbstractKeeper<ComposeFrameToTextureKeeperSeed>;

} // namespace Vitrae