#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

namespace Vitrae
{
class Renderer;
class FrameStore;
class Texture;

struct RenderRunContext
{
    ScopedDict &properties;
    Renderer &renderer;
    const std::map<StringId, dynasma::FirmPtr<FrameStore>> &preparedCompositorFrameStores;
    const std::map<StringId, dynasma::FirmPtr<Texture>> &preparedCompositorTextures;
};

class ComposeTask : public Task
{
  protected:
  public:
    using Task::Task;

    virtual void run(RenderRunContext args) const = 0;
    virtual void prepareRequiredFrameStores(
        std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores) const;
    virtual void prepareRequiredTextures(
        std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const;
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