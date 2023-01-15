#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/ResourceManager.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <map>
#include <set>

namespace Vitrae
{
    class SourceShaderStep;
    class GroupShaderStep;
    class SwitchShaderStep;

    /**
     * A ShaderStep describes a modular reusable part of a Shader
    */
    class ShaderStep
    {
    public:
        virtual ~ShaderStep() = 0;

        virtual void extractInputPropertyNames(std::set<String> &outNames) const = 0;
        virtual void extractInputVariableNames(std::set<String> &outNames) const = 0;
        virtual void extractOutputVariableNames(std::set<String> &outNames) const = 0;
        virtual void extractSourceShaderSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, ShaderProperty> &properties) = 0;

    };
    
}