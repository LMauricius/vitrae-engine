#pragma once

#include "Vitrae/Pipelines/Rendering/Task.hpp"
#include "Vitrae/Pipelines/Switch.h"

namespace Vitrae
{

class RenderSwitch : public Switch<RenderTask>
{
  public:
    using Switch<RenderTask>::Switch;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost

    void run(RenderRunContext args) override;
};
} // namespace Vitrae