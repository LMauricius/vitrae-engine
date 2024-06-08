#pragma once

#include "Vitrae/Pipelines/Shading/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <filesystem>
#include <variant>

namespace Vitrae
{

class ShaderFunction : public virtual ShaderTask
{
  public:
    struct SetupParams
    {
        std::vector<PropertySpec> inputSpecs;
        std::vector<PropertySpec> outputSpecs;
        std::filesystem::path filepath;
        String functionName;
    };

    inline ShaderFunction(const SetupParams &params)
        : ShaderTask(params.inputSpecs, params.outputSpecs)
    {}
};

struct ShaderFunctionKeeperSeed
{
    using Asset = ShaderFunction;
    std::variant<ShaderFunction::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ShaderFunctionKeeper = dynasma::AbstractKeeper<ShaderFunctionKeeperSeed>;

} // namespace Vitrae