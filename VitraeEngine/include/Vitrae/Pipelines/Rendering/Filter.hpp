#pragma once

#include "Vitrae/Pipelines/Filter.hpp"
#include "Vitrae/Pipelines/Rendering/Task.hpp"

namespace Vitrae
{

class RenderFilter : public Filter<RenderTask>
{
  public:
    using Filter<RenderTask>::Filter;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost

    void run(RenderRunContext args) override;
};
} // namespace Vitrae