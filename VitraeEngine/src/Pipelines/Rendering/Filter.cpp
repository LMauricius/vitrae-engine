#include "Vitrae/Pipelines/Rendering/Filter.hpp"

namespace Vitrae
{
void RenderFilter::run(RenderRunContext args)
{
    if (std::any_cast<FilterConditionType>(*args.inputPropertyPtrs.at(m_conditionProperty)))
    {
        m_task->run(args);
    }
}
} // namespace Vitrae