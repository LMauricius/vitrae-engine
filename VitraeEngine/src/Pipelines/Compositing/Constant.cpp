#include "Vitrae/Pipelines/Compositing/Constant.hpp"

namespace Vitrae
{
ComposeConstant::ComposeConstant(const SetupParams &params)
    : ComposeTask({}, {{params.outputSpec.name, params.outputSpec}}),
      m_outputNameId(params.outputSpec.name), m_outputSpec(params.outputSpec), m_value(params.value)
{}

void ComposeConstant::run(RenderRunContext args) const
{
    args.properties.set(m_outputNameId, m_value);
}

} // namespace Vitrae