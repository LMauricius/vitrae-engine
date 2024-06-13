#pragma once

#include "Vitrae/Pipelines/Compositing/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <variant>

namespace Vitrae
{

class ComposeConstant : public ComposeTask
{
  public:
    struct SetupParams
    {
        PropertySpec outputSpec;
        Variant value;
    };

    ComposeConstant(const SetupParams &params);
    ~ComposeConstant() = default;

    void run(RenderRunContext args) const override;
    void prepareRequiredLocalAssets(
        std::map<StringId, dynasma::FirmPtr<FrameStore>> &frameStores,
        std::map<StringId, dynasma::FirmPtr<Texture>> &textures) const override;

  protected:
    StringId m_outputNameId;
    PropertySpec m_outputSpec;
    Variant m_value;
};

struct ComposeConstantKeeperSeed
{
    using Asset = ComposeConstant;
    std::variant<ComposeConstant::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using ComposeConstantKeeper = dynasma::AbstractKeeper<ComposeConstantKeeperSeed>;

} // namespace Vitrae