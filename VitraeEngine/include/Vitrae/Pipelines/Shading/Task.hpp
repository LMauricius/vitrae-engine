#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

#include "dynasma/util/dynamic_typing.hpp"

#include <functional>
#include <vector>

namespace Vitrae
{

struct ShaderBuildContext
{
    std::stringstream &output;
};

struct ShaderRunContext
{
    const ScopedDict &properties;
};

struct ShaderSetupContext
{
    std::vector<std::function<void(ShaderRunContext)>> setupFunctions;
};

class ShaderTask : public Task
{
  protected:
  public:
    virtual void outputDeclarationCode(ShaderBuildContext args) const = 0;
    virtual void outputDefinitionCode(ShaderBuildContext args) const = 0;
    virtual void outputUsageCode(
        ShaderBuildContext args, const std::map<StringId, String> &inputParamsToSharedVariables,
        const std::map<StringId, String> &outputParamsToSharedVariables) const = 0;
    virtual void hookSetupFunctions(ShaderSetupContext args) const = 0;
};

} // namespace Vitrae