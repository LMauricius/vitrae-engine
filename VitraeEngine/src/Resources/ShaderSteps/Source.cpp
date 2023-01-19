#include "Vitrae/Resources/ShaderSteps/Source.h"

#include <fstream>

namespace Vitrae
{

    SourceShaderStep::SourceShaderStep()
    {
        
    }

    SourceShaderStep::~SourceShaderStep()
    {

    }

    void SourceShaderStep::load(const ShaderFileParams& params)
    {
        std::ifstream file;
        file.open(params.shaderFilepath);

        mSrcString.assign(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        
        mInputPropertyNames = params.propertyNames;
        mInputVariableNames = params.inputVariableNames;
        mOutputVariableNames = params.outputVariableNames;
    }

    void SourceShaderStep::extractInputPropertyNames(std::set<String> &outNames) const
    {
        outNames.insert(mInputPropertyNames.begin(), mInputPropertyNames.end());
    }
    
    void SourceShaderStep::extractInputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
    {
        outSpecs.insert(mInputVariableNames.begin(), mInputVariableNames.end());
    }
    
    void SourceShaderStep::extractOutputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
    {
        if (!enablingCondition) {
            outSpecs.insert(mOutputVariableNames.begin(), mOutputVariableNames.end());
        }
    }
    
    void SourceShaderStep::extractPrimitiveSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, VariantProperty> &properties)
    {
        if (!enablingCondition || enablingCondition(properties)) {
            outSteps.push_back(this);
        }
    }

    void SourceShaderStep::extractSource(std::ostream &os)
    {
        os << mSrcString;
    }
    
}