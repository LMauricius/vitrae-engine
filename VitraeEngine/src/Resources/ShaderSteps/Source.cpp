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
    
    void SourceShaderStep::extractInputVariableNames(std::set<String> &outNames) const
    {
        outNames.insert(mInputVariableNames.begin(), mInputVariableNames.end());
    }
    
    void SourceShaderStep::extractOutputVariableNames(std::set<String> &outNames) const
    {
        outNames.insert(mOutputVariableNames.begin(), mOutputVariableNames.end());
    }
    
    void SourceShaderStep::extractSourceShaderSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, ShaderProperty> &properties)
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