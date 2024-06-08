#pragma once

#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

#include <functional>
#include <vector>

namespace Vitrae
{

class OpenGLRenderer;

class OpenGLShaderTask : public virtual ShaderTask
{
  public:
    using ShaderTask::ShaderTask;

    struct BuildContext
    {
        std::stringstream &output;
        OpenGLRenderer &renderer;
    };

    struct RunContext
    {
        const ScopedDict &properties;
        OpenGLRenderer &renderer;
    };

    struct SetupContext
    {
        std::vector<std::function<void(RunContext)>> setupFunctions;
        OpenGLRenderer &renderer;
    };

    virtual void outputDeclarationCode(BuildContext args) const = 0;
    virtual void outputDefinitionCode(BuildContext args) const = 0;
    virtual void outputUsageCode(
        BuildContext args, const std::map<StringId, String> &inputParamsToSharedVariables,
        const std::map<StringId, String> &outputParamsToSharedVariables) const = 0;
    virtual void hookSetupFunctions(SetupContext args) const = 0;
};

} // namespace Vitrae