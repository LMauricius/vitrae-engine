#include "Vitrae/Pipelines/Compositing/FrameToTexture.hpp"
#include "Vitrae/Assets/FrameStore.hpp"

#include <span>

namespace Vitrae
{
ComposeFrameToTexture::ComposeFrameToTexture(const SetupParams &params)
    : ComposeTask(
          std::span<const PropertySpec>{{{params.frameInputPropertyName,
                                          Variant::getTypeInfo<dynasma::FirmPtr<FrameStore>>()}}},
          (params.colorTextureOutputPropertyName != "")
              ? (params.depthTextureOutputPropertyName != "")
                    ? std::span<
                          const PropertySpec>{{{params.colorTextureOutputPropertyName,
                                                Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()},
                                               {params.depthTextureOutputPropertyName,
                                                Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()}}}
                    : std::span<
                          const PropertySpec>{{{params.colorTextureOutputPropertyName,
                                                Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()}}}
          : (params.depthTextureOutputPropertyName != "")
              ? std::span<const PropertySpec>{{{params.depthTextureOutputPropertyName,
                                                Variant::getTypeInfo<dynasma::FirmPtr<Texture>>()}}}
              : std::span<const PropertySpec>()),
      m_root(params.root), m_frameInputNameId(params.frameInputPropertyName),
      m_colorTextureOutputNameId(params.colorTextureOutputPropertyName),
      m_depthTextureOutputNameId(params.depthTextureOutputPropertyName), m_size(params.size),
      m_channelType(params.channelType), m_horWrap(params.horWrap), m_verWrap(params.verWrap),
      m_minFilter(params.minFilter), m_magFilter(params.magFilter), m_useMipMaps(params.useMipMaps),
      m_borderColor(params.borderColor)

{}

void ComposeFrameToTexture::run(RenderRunContext args) const
{
    if (m_colorTextureOutputNameId != "") {
        args.properties.set(m_colorTextureOutputNameId,
                            args.preparedCompositorTextures.at(m_colorTextureOutputNameId));
    }
    if (m_depthTextureOutputNameId != "") {
        args.properties.set(m_depthTextureOutputNameId,
                            args.preparedCompositorTextures.at(m_depthTextureOutputNameId));
    }
}

void ComposeFrameToTexture::prepareRequiredLocalAssets(
    std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
    std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const
{
    FrameStoreManager &frameManager = m_root.getComponent<FrameStoreManager>();
    TextureManager &textureManager = m_root.getComponent<TextureManager>();

    FrameStore::TextureBindParams frameParams = {m_root, {}, {}};

    if (m_colorTextureOutputNameId != "") {
        auto texture =
            textureManager.register_asset({Texture::EmptyParams{.root = m_root,
                                                                .size = m_size,
                                                                .channelType = m_channelType,
                                                                .horWrap = m_horWrap,
                                                                .verWrap = m_verWrap,
                                                                .minFilter = m_minFilter,
                                                                .magFilter = m_magFilter,
                                                                .useMipMaps = m_useMipMaps,
                                                                .borderColor = m_borderColor}});
        frameParams.p_colorTexture = texture;
        textures.emplace(m_colorTextureOutputNameId, texture);
    }
    if (m_depthTextureOutputNameId != "") {
        auto texture = textureManager.register_asset(
            {Texture::EmptyParams{.root = m_root,
                                  .size = m_size,
                                  .channelType = Texture::ChannelType::DEPTH,
                                  .horWrap = m_horWrap,
                                  .verWrap = m_verWrap,
                                  .minFilter = m_minFilter,
                                  .magFilter = m_magFilter,
                                  .useMipMaps = m_useMipMaps,
                                  .borderColor = {1.0f, 1.0f, 1.0f, 1.0f}}});
        frameParams.p_depthTexture = texture;
        textures.emplace(m_depthTextureOutputNameId, texture);
    }

    auto frame = frameManager.register_asset({frameParams});
    frameStores.emplace(m_frameInputNameId, frame);
}

} // namespace Vitrae