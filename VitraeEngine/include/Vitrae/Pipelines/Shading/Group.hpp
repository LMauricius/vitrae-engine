#pragma once

#include "Vitrae/Pipelines/Group.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"

namespace Vitrae
{

class ShaderGroup : public Group<ShaderTask>
{
  public:
    using Group<ShaderTask>::Group;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost
};
} // namespace Vitrae