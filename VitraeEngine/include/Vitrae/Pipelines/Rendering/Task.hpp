#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "Vitrae/Util/ScopedDict.hpp"

namespace Vitrae
{

struct RenderRunContext
{
    const ScopedDict &properties;
    const std::map<StringId, const Property *> &inputPropertyPtrs;
    const std::map<StringId, Property *> &outputPropertyPtrs;
};

class RenderTask : public Task
{
  protected:
  public:
    virtual void run(RenderRunContext args) = 0;
};

} // namespace Vitrae