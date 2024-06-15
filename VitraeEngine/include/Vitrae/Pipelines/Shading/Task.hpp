#pragma once

#include "Vitrae/Pipelines/Task.hpp"
#include "Vitrae/Util/ScopedDict.hpp"

#include "glm/glm.hpp"

namespace Vitrae
{

class Renderer;

class ShaderTask : public Task
{
  public:
    struct BuildContext
    {
        std::stringstream &output;
        Renderer &renderer;
    };

    struct RunContext
    {
        const ScopedDict &properties;
        Renderer &renderer;
    };

    struct SetupContext
    {
        std::vector<std::function<void(RunContext)>> setupFunctions;
        Renderer &renderer;
    };

    using Task::Task;

    virtual void outputDeclarationCode(BuildContext args) const = 0;
    virtual void outputDefinitionCode(BuildContext args) const = 0;
    virtual void outputUsageCode(
        BuildContext args, const std::map<StringId, String> &inputParamsToSharedVariables,
        const std::map<StringId, String> &outputParamsToSharedVariables) const = 0;
};

namespace StandardShaderPropertyNames
{
static constexpr const char INPUT_VIEW[] = "mat_view";
static constexpr const char INPUT_PROJECTION[] = "mat_proj";
static constexpr const char INPUT_MODEL[] = "mat_model";

static constexpr const char FRAGMENT_OUTPUT[] = "shade";
static constexpr const char VERTEX_OUTPUT[] = "view_position";
static constexpr const char COMPUTE_OUTPUT[] = "computed_data";
} // namespace StandardShaderOutputNames

namespace StandardShaderPropertyTypes
{
static constexpr const TypeInfo &INPUT_VIEW = Variant::getTypeInfo<glm::mat4>();
static constexpr const TypeInfo &INPUT_PROJECTION = Variant::getTypeInfo<glm::mat4>();
static constexpr const TypeInfo &INPUT_MODEL = Variant::getTypeInfo<glm::mat4>();

static constexpr const TypeInfo &FRAGMENT_OUTPUT = Variant::getTypeInfo<glm::vec4>();
static constexpr const TypeInfo &VERTEX_OUTPUT = Variant::getTypeInfo<glm::vec3>();
}

} // namespace Vitrae