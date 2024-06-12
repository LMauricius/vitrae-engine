#include "Vitrae/Renderers/OpenGL/Compositing/SceneRender.hpp"
#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Visuals/Scene.hpp"

namespace Vitrae
{

OpenGLComposeSceneRender::OpenGLComposeSceneRender(const SetupParams &params)
    : ComposeSceneRender(
          std::span<const PropertySpec>{
              {PropertySpec{.name = params.sceneInputPropertyName,
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()}}},
          std::span<const PropertySpec>{
              {PropertySpec{.name = params.displayOutputPropertyName,
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<FrameStore>>()}}}),
      m_sceneInputNameId(params.sceneInputPropertyName),
      m_displayOutputNameId(params.displayOutputPropertyName)
{}

void OpenGLComposeSceneRender::run(RenderRunContext args) const {}

void OpenGLComposeSceneRender::prepareRequiredFrameStores(
    std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores) const
{
    // We just need to check whether the frame store is already prepared
    if (frameStores.find(m_displayOutputNameId) == frameStores.end()) {
        throw std::runtime_error("Frame store not found");
    }
}

void OpenGLComposeSceneRender::prepareRequiredTextures(
    std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const
{}

} // namespace Vitrae