#pragma once

#include "Vitrae/Pipelines/Group.h"
#include "Vitrae/Pipelines/Rendering/Task.hpp"

namespace Vitrae
{

class RenderGroup : public Group<RenderTask>
{
  public:
    using Group<RenderTask>::Group;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost

    void run(RenderRunContext args) override;
};
} // namespace Vitrae