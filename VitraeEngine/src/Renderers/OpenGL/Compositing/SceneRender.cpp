#include "Vitrae/Renderers/OpenGL/Compositing/SceneRender.hpp"
#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Visuals/Scene.hpp"

namespace Vitrae
{

OpenGLComposeSceneRender::OpenGLComposeSceneRender(const SetupParams &params)
    : ComposeSceneRender(
          std::span<const PropertySpec>{
              {PropertySpec{.name = "scene",
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<Scene>>()},
               PropertySpec{.name = params.viewInputPropertyName,
                            .typeInfo = Variant::getTypeInfo<glm::mat4>()},
               PropertySpec{.name = params.perspectiveInputPropertyName,
                            .typeInfo = Variant::getTypeInfo<glm::mat4>()}}},
          std::span<const PropertySpec>{
              {PropertySpec{.name = params.displayOutputPropertyName,
                            .typeInfo = Variant::getTypeInfo<dynasma::FirmPtr<FrameStore>>()}}}),
      m_root(params.root), m_viewInputNameId(params.viewInputPropertyName),
      m_perspectiveInputNameId(params.perspectiveInputPropertyName),
      m_displayOutputNameId(params.displayOutputPropertyName)
{}

void OpenGLComposeSceneRender::run(RenderRunContext args) const
{
    Scene &scene = *args.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
    glm::mat4 viewMat = args.properties.get(m_viewInputNameId).get<glm::mat4>();
    glm::mat4 perspectiveMat = args.properties.get(m_perspectiveInputNameId).get<glm::mat4>();
}

void OpenGLComposeSceneRender::prepareRequiredLocalAssets(
    std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
    std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const
{
    // We just need to check whether the frame store is already prepared
    if (frameStores.find(m_displayOutputNameId) == frameStores.end()) {
        throw std::runtime_error("Frame store not found");
    }
}

} // namespace Vitrae