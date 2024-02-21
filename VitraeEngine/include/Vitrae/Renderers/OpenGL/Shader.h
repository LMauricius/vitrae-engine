#pragma once

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

    class GLSLShader
    {
    public:
        struct OutputData
        {
            dynasma::LazyPtr<ShaderStep> mainStep;
            std::set<String> inputPropertyNames;
            std::map<String, VariantPropertySpec> inputVariableNames;
            std::map<String, VariantPropertySpec> outputVariableNames;
        };

        GLSLShader();
        ~GLSLShader();

        void setOutputStep(const String &outputName, dynasma::LazyPtr<ShaderStep> step);
        const OutputData &getOutputData(const String& outputName) const;

    protected:
        std::map<String, OutputData> mNamedOutputData;
    };

    struct CompiledGLSLProgram
    {
        struct UniformSpec
        {
            VariantPropertySpec srcSpec;
            GLint uniformGLName;
        };
        GLuint programGLName;
        std::map<String, UniformSpec> uniformSpecs;

        CompiledGLSLProgram(
            const GLSLShader::OutputData &geom, const GLSLShader::OutputData &vertex, const GLSLShader::OutputData &fragment,
            const std::map<String, VariantProperty> &properties,
            const OpenGLRenderer &rend, AssetRoot &resRoot);
        ~CompiledGLSLProgram();


    };
    
}