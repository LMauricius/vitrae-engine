#pragma once

#include "Vitrae/Assets/Shader.hpp"
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
class AssetRoot;

class CompiledGLSLShader : public CompiledShader
{
  public:
    struct VariableSpec
    {
        const TypeInfo &srcSpec;
        GLint glNameId;
    };

    CompiledGLSLShader(const ShaderCompilationParams &params);
    ~CompiledGLSLShader();

    GLuint programGLName;
    std::map<StringId, VariableSpec> uniformSpecs;
    std::map<StringId, VariableSpec> uboSpecs;
    std::map<StringId, VariableSpec> ssboSpecs;
};
} // namespace Vitrae