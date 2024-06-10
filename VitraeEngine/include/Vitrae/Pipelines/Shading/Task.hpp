#pragma once

#include "Vitrae/Pipelines/Task.hpp"

#include "glm/glm.hpp"

namespace Vitrae
{
class ShaderTask : public Task
{
    using Task::Task;
};

namespace StandardShaderOutputNames
{
constexpr const char SURFACE_SHADER_OUTPUT[] = "fragment_shade";
constexpr const char COMPUTE_SHADER_OUTPUT[] = "computed_data";
} // namespace StandardShaderOutputNames

namespace StandardShaderOutputTypes
{
constexpr const TypeInfo &SURFACE_SHADER_OUTPUT_TYPE = Variant::getTypeInfo<glm::vec4>();
}

} // namespace Vitrae