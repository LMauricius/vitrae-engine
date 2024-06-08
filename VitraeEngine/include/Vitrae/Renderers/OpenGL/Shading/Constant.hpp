#pragma once

#include "Vitrae/Pipelines/Shading/Constant.hpp"
#include "Vitrae/Renderers/OpenGL/Shading/Task.hpp"

#include "dynasma/keepers/abstract.hpp"

#include <filesystem>
#include <variant>

namespace Vitrae
{

class OpenGLShaderConstant : public ShaderConstant, public OpenGLShaderTask
{
  public:
    OpenGLShaderConstant(const SetupParams &params);
    ~OpenGLShaderConstant() = default;

    std::size_t memory_cost() const override;

    void outputDeclarationCode(BuildContext args) const override;
    void outputDefinitionCode(BuildContext args) const override;
    void outputUsageCode(
        BuildContext args, const std::map<StringId, String> &inputParamsToSharedVariables,
        const std::map<StringId, String> &outputParamsToSharedVariables) const override;
    void hookSetupFunctions(SetupContext args) const override;
};

} // namespace Vitrae