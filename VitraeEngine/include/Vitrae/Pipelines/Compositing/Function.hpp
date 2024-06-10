#pragma once

#include "Vitrae/Pipelines/Compositing/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <functional>

namespace Vitrae
{

class ComposeFunction : public virtual ComposeTask
{
    std::function<void(const RenderRunContext &)> mp_function;

  public:
    struct SetupParams
    {
        std::vector<PropertySpec> inputSpecs;
        std::vector<PropertySpec> outputSpecs;
        std::function<void(const RenderRunContext &)> p_function;
    };

    ComposeFunction(const SetupParams &params);
    ~ComposeFunction() = default;

    void run(RenderRunContext args) const override;
};

struct ComposeFunctionKeeperSeed
{
    using Asset = ComposeFunction;
    std::variant<ComposeFunction::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ComposeFunctionKeeper = dynasma::AbstractKeeper<ComposeFunctionKeeperSeed>;
} // namespace Vitrae