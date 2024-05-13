#pragma once

#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Pipelines/Switch.h"

namespace Vitrae
{

class ShaderSwitch : public Switch<ShaderTask>
{
  public:
    using Switch<ShaderTask>::Switch;

    inline std::size_t memory_cost() const override
    {
        return sizeof(*this);
    } /// TODO: compute the real cost
};
} // namespace Vitrae