#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

namespace Vitrae
{

struct RenderRunContext
{
    const ScopedDict &properties;
    const std::map<StringId, const Variant *> &inputPropertyPtrs;
    const std::map<StringId, Variant *> &outputPropertyPtrs;
};

class RenderTask : public Task
{
  protected:
  public:
    virtual void run(RenderRunContext args) = 0;
};

} // namespace Vitrae