#pragma once

#include "Vitrae/Pipelines/Task.hpp"

#include "glm/glm.hpp"

namespace Vitrae
{
class ShaderTask : public Task
{
    using Task::Task;
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