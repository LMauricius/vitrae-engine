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

} // namespace Vitrae