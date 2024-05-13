#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "Vitrae/Util/ScopedDict.hpp"

#include <functional>
#include <vector>

namespace Vitrae
{

struct ShaderBuildContext
{
    std::ostringstream &output;
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
    virtual void outputDeclarationCode(ShaderBuildContext args) = 0;
    virtual void outputDefinitionCode(ShaderBuildContext args) = 0;
    virtual void outputUsageCode(
        ShaderBuildContext args, const std::map<StringId, StringId> &inputParamsToSharedVariables,
        const std::map<StringId, StringId> &outputParamsToSharedVariables) = 0;
    virtual void hookSetupFunctions(ShaderSetupContext args) = 0;
};

} // namespace Vitrae