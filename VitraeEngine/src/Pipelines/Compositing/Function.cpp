#include "Vitrae/Pipelines/Compositing/Function.hpp"

namespace Vitrae
{

ComposeFunction::ComposeFunction(const SetupParams &params)
    : ComposeTask(params.inputSpecs, params.outputSpecs), mp_function(params.p_function)
{}

void ComposeFunction::run(RenderRunContext args) const
{
    mp_function(args);
}

void ComposeFunction::prepareRequiredFrameStores(
    std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores) const
{}

void ComposeFunction::prepareRequiredTextures(
    std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const
{}

} // namespace Vitrae