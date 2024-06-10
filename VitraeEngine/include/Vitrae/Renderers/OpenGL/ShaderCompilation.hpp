#pragma once

#include "Vitrae/Pipelines/Pipeline.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Types/Typedefs.hpp"
#include "Vitrae/Util/Property.hpp"

#include "dynasma/pointer.hpp"
#include "glad/glad.h"

#include <map>
#include <set>

namespace Vitrae
{
class OpenGLRenderer;
class ShaderStep;
class ComponentRoot;

class CompiledGLSLShader
{
  public:
    struct CompilationSpec
    {
        // the specified shading method
        dynasma::FirmPtr<Method<ShaderTask>> p_method;

        // prefix for output variables
        String outVarPrefix;

        // gl shader type
        GLenum shaderType;
    };

    struct SurfaceShaderSpec
    {
        dynasma::FirmPtr<Method<ShaderTask>> vertexMethod;
        dynasma::FirmPtr<Method<ShaderTask>> fragmentMethod;
        ComponentRoot &root;
    };

    struct VariableSpec
    {
        const TypeInfo &srcSpec;
        GLint glNameId;
    };

    CompiledGLSLShader(std::span<const CompilationSpec> compilationSpecs, ComponentRoot &root);
    CompiledGLSLShader(const SurfaceShaderSpec &params);
    ~CompiledGLSLShader();

    GLuint programGLName;
    std::map<StringId, VariableSpec> uniformSpecs;
    std::map<StringId, VariableSpec> uboSpecs;
    std::map<StringId, VariableSpec> ssboSpecs;
};
} // namespace Vitrae