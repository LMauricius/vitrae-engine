#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

namespace Vitrae
{

class Renderer;

struct RenderRunContext
{
    ScopedDict &properties;
    Renderer &renderer;
};

class ComposeTask : public Task
{
  protected:
  public:
    virtual void run(RenderRunContext args) = 0;
};

} // namespace Vitrae