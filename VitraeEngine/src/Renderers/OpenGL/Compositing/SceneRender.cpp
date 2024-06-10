#include "Vitrae/Renderers/OpenGL/Compositing/SceneRender.hpp"

namespace Vitrae
{

OpenGLComposeSceneRender::OpenGLComposeSceneRender(const SetupParams &params)
    : ComposeSceneRender(std::span<const PropertySpec>{},
                         std::span<const PropertySpec>{{PropertySpec{
                             .name = StandardComposeOutputNames::RENDER_OUTPUT,
                             .typeInfo = StandardComposeOutputTypes::SURFACE_SHADER_OUTPUT_TYPE}}})
{}

void OpenGLComposeSceneRender::run(RenderRunContext args) const {}

} // namespace Vitrae