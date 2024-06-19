#pragma once

#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"

namespace Vitrae
{

class OpenGLRenderer;

class OpenGLComposeClearRender : public ComposeClearRender
{
  public:
    using ComposeClearRender::ComposeClearRender;

    OpenGLComposeClearRender(const SetupParams &params);

    void run(RenderRunContext args) const override;
    void prepareRequiredLocalAssets(
        std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
        std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const override;

  protected:
    ComponentRoot &m_root;
    glm::vec4 m_color;
    StringId m_displayOutputNameId;
    std::optional<StringId> m_displayInputNameId;
};

} // namespace Vitrae