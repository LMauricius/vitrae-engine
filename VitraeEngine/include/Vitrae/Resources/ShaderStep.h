#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/ResourceManager.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <span>
#include <map>

namespace Vitrae
{
    class SourceShaderStep;

    /**
     * A ShaderStep describes a modular reusable part of a Shader
    */
    class ShaderStep
    {
    public:
        virtual ~ShaderStep() = 0;

        virtual std::span<String> getInputPropertyNames() const = 0;
        virtual std::span<String> getInputVariableNames() const = 0;
        virtual std::span<String> getOutputVariableNames() const = 0;

        virtual bool isEnabled(const std::map<String, ShaderProperty> &properties) = 0;
        virtual void extractSourceShaderSteps(std::vector<SourceShaderStep> &outSteps, const std::map<String, ShaderProperty> &properties) = 0;

    };
    
}