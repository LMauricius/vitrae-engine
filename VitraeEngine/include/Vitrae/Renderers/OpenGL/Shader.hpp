#pragma once

#include "Vitrae/Assets/Shader.h"
#include "Vitrae/Types/Typedefs.h"
#include "Vitrae/Util/Property.h"

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