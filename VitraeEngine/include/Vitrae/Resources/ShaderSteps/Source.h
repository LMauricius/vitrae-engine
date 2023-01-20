#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/ResourceManager.h"
#include "Vitrae/Resources/ShaderStep.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <ostream>
#include <span>
#include <map>
#include <functional>
#include <variant>
#include <filesystem>

namespace Vitrae
{
    /**
     * A SourceShaderStep is a cornerstone step type
     * that describes a shading step loaded from a shading language file
    */
    class SourceShaderStep: public ShaderStep
    {
    public:
        struct ShaderFileParams {
            std::filesystem::path shaderFilepath;
            std::set<String> propertyNames;
            std::set<String> inputVariableNames;
            std::set<String> outputVariableNames;
        };

        using SetupParams = std::variant<ShaderFileParams>;
        using LoadParams = std::variant<EmptyType>;

        SourceShaderStep();
        ~SourceShaderStep();

        void load(const ShaderFileParams& params);

        void extractInputPropertyNames(std::set<String> &outNames) const;
        void extractInputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const;
        void extractOutputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const;
        void extractPrimitiveSteps(std::vector<const SourceShaderStep*> &outSteps, const std::map<String, VariantProperty> &properties) const;
        void extractSource(std::ostream &os) const;

        std::function<bool(const std::map<String, VariantProperty> &properties)> enablingCondition;

    protected:
        std::set<String> mInputPropertyNames;
        std::set<String> mInputVariableNames;
        std::set<String> mOutputVariableNames;
        String mSrcString;
    };
    
}