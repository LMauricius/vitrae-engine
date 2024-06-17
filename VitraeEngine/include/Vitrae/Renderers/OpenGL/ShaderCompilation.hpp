#pragma once

#include "Vitrae/Pipelines/Pipeline.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Types/Typedefs.hpp"
#include "Vitrae/Util/Variant.hpp"

#include "dynasma/cachers/abstract.hpp"
#include "dynasma/pointer.hpp"
#include "glad/glad.h"

#include <map>
#include <set>

namespace Vitrae
{
class OpenGLRenderer;
class ComponentRoot;

class CompiledGLSLShader : public dynasma::PolymorphicBase
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

        inline bool operator<(const SurfaceShaderSpec &o) const
        {
            return std::tie(vertexMethod, fragmentMethod) <
                   std::tie(o.vertexMethod, o.fragmentMethod);
        }
    };

    struct VariableSpec
    {
        const TypeInfo &srcSpec;
        GLint glNameId;
    };

    CompiledGLSLShader(std::span<const CompilationSpec> compilationSpecs, ComponentRoot &root);
    CompiledGLSLShader(const SurfaceShaderSpec &params);
    ~CompiledGLSLShader();

    inline std::size_t memory_cost() const { return 1; }

    GLuint programGLName;
    std::map<StringId, VariableSpec> uniformSpecs;
    std::map<StringId, VariableSpec> bindingSpecs;
    std::map<StringId, VariableSpec> uboSpecs;
    std::map<StringId, VariableSpec> ssboSpecs;
};

struct CompiledGLSLShaderCacherSeed
{
    using Asset = CompiledGLSLShader;

    std::variant<CompiledGLSLShader::SurfaceShaderSpec> kernel;

    inline std::size_t load_cost() const { return 1; }

    inline bool operator<(const CompiledGLSLShaderCacherSeed &o) const { return kernel < o.kernel; }
};

using CompiledGLSLShaderCacher = dynasma::AbstractCacher<CompiledGLSLShaderCacherSeed>;

} // namespace Vitrae