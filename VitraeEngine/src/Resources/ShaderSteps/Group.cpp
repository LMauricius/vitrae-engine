#include "Vitrae/Resources/ShaderSteps/Group.h"

namespace Vitrae
{

    GroupShaderStep::GroupShaderStep()
    {

    }

    GroupShaderStep::~GroupShaderStep()
    {

    }

    void GroupShaderStep::extractInputPropertyNames(std::set<String> &outNames) const
    {
        for (auto &step : stepList) {
            step->extractInputPropertyNames(outNames);
        }
    }
    
    void GroupShaderStep::extractInputVariableNames(std::set<String> &outNames) const
    {
        for (auto &step : stepList) {
            step->extractInputVariableNames(outNames);
        }
    }
    
    void GroupShaderStep::extractOutputVariableNames(std::set<String> &outNames) const
    {
        for (auto &step : stepList) {
            step->extractOutputVariableNames(outNames);
        }
    }
    
    void GroupShaderStep::extractSourceShaderSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, ShaderProperty> &properties)
    {
        if (!enablingCondition || enablingCondition(properties)) {
            for (auto &step : stepList) {
                step->extractSourceShaderSteps(outSteps, properties);
            }
        }
    } 

}