#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/ResourceManager.h"
#include "Vitrae/Resources/ShaderStep.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <span>
#include <map>
#include <functional>
#include <optional>

namespace Vitrae
{
    /**
     * A ShaderStep describes a modular reusable part of a Shader
    */
    class SwitchShaderStep: public ShaderStep
    {
    public:
        struct SetupParams {};
        struct LoadParams {};

        struct Case
        {
            casted_resource_ptr<ShaderStep> choice;
            std::function<bool(const std::map<String, ShaderProperty> &properties)> enablingCondition;
        };

        SwitchShaderStep();
        ~SwitchShaderStep();

        void extractInputPropertyNames(std::set<String*> &outNames) const;
        void extractInputVariableNames(std::set<String*> &outNames) const;
        void extractOutputVariableNames(std::set<String*> &outNames) const;
        void extractSourceShaderSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, ShaderProperty> &properties);

        std::vector<Case> cases;
        std::optional<casted_resource_ptr<ShaderStep>> defaultChoice;
    };
    
}