#pragma once

#include "Vitrae/Pipelines/Compositing/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <functional>

namespace Vitrae
{

class ComposeSceneRender : public ComposeTask
{
  public:
    struct SetupParams
    {
        ComponentRoot &root;
        String viewInputPropertyName;
        String perspectiveInputPropertyName;
        String displayInputPropertyName;
        String displayOutputPropertyName;
        PropertyList customInputPropertyNames;
    };

    using ComposeTask::ComposeTask;
};

struct ComposeSceneRenderKeeperSeed
{
    using Asset = ComposeSceneRender;
    std::variant<ComposeSceneRender::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ComposeSceneRenderKeeper = dynasma::AbstractKeeper<ComposeSceneRenderKeeperSeed>;
} // namespace Vitrae