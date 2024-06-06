#pragma once

#include "Vitrae/Pipelines/Filter.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"

namespace Vitrae
{

class ShaderFilter : public Filter<ShaderTask>
{
  public:
    using Filter<ShaderTask>::Filter;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost
};
} // namespace Vitrae