#pragma once

#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

#include <functional>
#include <vector>

namespace Vitrae
{

class OpenGLRenderer;

class OpenGLComposeSceneRender : public ComposeSceneRender
{
  public:
    using ComposeSceneRender::ComposeSceneRender;

    OpenGLComposeSceneRender(const SetupParams &params);

    void run(RenderRunContext args) const override;
    void prepareRequiredLocalAssets(
        std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
        std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const override;

  protected:
    ComponentRoot &m_root;
    StringId m_viewInputNameId, m_perspectiveInputNameId, m_displayOutputNameId;
};

} // namespace Vitrae