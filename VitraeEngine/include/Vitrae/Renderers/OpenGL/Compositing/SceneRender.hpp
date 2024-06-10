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
};

} // namespace Vitrae