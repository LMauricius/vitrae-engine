#pragma once

#include "Vitrae/Resources/Shader.h"

#include "glad/glad.h"

#include <map>
#include <set>

namespace Vitrae
{
    class OpenGLRenderer;
    class ShaderStep;
    class ResourceRoot;

    class GLSLShader: public Shader
    {
    public:
        struct OutputData
        {
            casted_resource_ptr<ShaderStep> mainStep;
            std::set<String> inputPropertyNames;
            std::map<String, VariantPropertySpec> inputVariableNames;
            std::map<String, VariantPropertySpec> outputVariableNames;
        };

        GLSLShader();
        ~GLSLShader();

        void setOutputStep(const String& outputName, casted_resource_ptr<ShaderStep> step);
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
            const OpenGLRenderer &rend, ResourceRoot &resRoot
        );
        ~CompiledGLSLProgram();


    };
    
}