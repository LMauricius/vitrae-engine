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

namespace StandardCompositorOutputNames
{
constexpr const char OUTPUT[] = "rendered_scene";
} // namespace StandardComposeOutputNames

namespace StandardCompositorOutputTypes
{
constexpr const TypeInfo &OUTPUT_TYPE = Variant::getTypeInfo<dynasma::FirmPtr<FrameStore>>();
}

} // namespace Vitrae