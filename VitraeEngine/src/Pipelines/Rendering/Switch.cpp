#include "Vitrae/Pipelines/Rendering/Switch.hpp"

namespace Vitrae
{
void RenderSwitch::run(RenderRunContext args)
{
    m_taskMap.at(std::any_cast<SwitchEnumType>(*args.inputPropertyPtrs.at(m_selectionProperty)))
        ->run(args);
}
} // namespace Vitrae