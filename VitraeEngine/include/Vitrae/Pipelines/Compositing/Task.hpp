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
    using Task::Task;

    virtual void run(RenderRunContext args) const = 0;
};

namespace StandardComposeOutputNames
{
constexpr const char RENDER_OUTPUT[] = "display";
} // namespace StandardComposeOutputNames

namespace StandardComposeOutputTypes
{
constexpr const TypeInfo &SURFACE_SHADER_OUTPUT_TYPE = Variant::getTypeInfo<void>();
}

} // namespace Vitrae