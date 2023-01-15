#include "Vitrae/Resources/ShaderSteps/Switch.h"

namespace Vitrae
{

    SwitchShaderStep::SwitchShaderStep()
    {

    }

    SwitchShaderStep::~SwitchShaderStep()
    {

    }

    void SwitchShaderStep::extractInputPropertyNames(std::set<String> &outNames) const
    {
        for (auto &c : cases) {
            c.choice->extractInputPropertyNames(outNames);
        }
        if (defaultChoice.has_value()) {
            defaultChoice.value()->extractInputPropertyNames(outNames);
        }
    }
    
    void SwitchShaderStep::extractInputVariableNames(std::set<String> &outNames) const
    {
        for (auto &c : cases) {
            c.choice->extractInputVariableNames(outNames);
        }
        if (defaultChoice.has_value()) {
            defaultChoice.value()->extractInputVariableNames(outNames);
        }
    }
    
    void SwitchShaderStep::extractOutputVariableNames(std::set<String> &outNames) const
    {
        for (auto &c : cases) {
            c.choice->extractOutputVariableNames(outNames);
        }
        if (defaultChoice.has_value()) {
            defaultChoice.value()->extractOutputVariableNames(outNames);
        }
    }
    
    void SwitchShaderStep::extractSourceShaderSteps(std::vector<SourceShaderStep*> &outSteps, const std::map<String, ShaderProperty> &properties)
    {
        for (auto &c : cases) {
            if (!c.enablingCondition || c.enablingCondition(properties)) {
                c.choice->extractSourceShaderSteps(outSteps, properties);
                return;
            }
        }
        if (defaultChoice.has_value()) {
            defaultChoice.value()->extractSourceShaderSteps(outSteps, properties);
        }
    }   

}