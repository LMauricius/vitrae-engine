#pragma once

#include "Vitrae/Pipelines/Shading/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <variant>

namespace Vitrae
{

class ShaderConstant : public ShaderTask
{
    StringId outputNameId;
    PropertySpec outputSpec;
    Variant value;

  public:
    struct SetupParams
    {
        PropertySpec outputSpec;
        Variant value;
    };

    inline ShaderConstant(const SetupParams &params)
        : ShaderTask({}, {{params.outputSpec.name, params.outputSpec}}),
          outputNameId(params.outputSpec.name), outputSpec(params.outputSpec), value(params.value)
    {}
};

struct ShaderConstantKeeperSeed
{
    using Asset = ShaderConstant;
    std::variant<ShaderConstant::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ShaderConstantKeeper = dynasma::AbstractKeeper<ShaderConstantKeeperSeed>;

} // namespace Vitrae