#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/AssetManager.h"

#include <span>
#include <map>
#include <functional>

namespace Vitrae
{
    
    template<class BaseStepT, class PrimitiveStepT>
    class GenericGroupStep: public ShaderStep
    {
    public:
        struct SetupParams {};
        struct LoadParams {};

        GenericGroupStep() = default;
        ~GenericGroupStep() = default;

        void extractInputPropertyNames(std::set<String> &outNames) const
        {
            for (auto &step : stepList) {
                step->extractInputPropertyNames(outNames);
            }
        }
        
        void extractInputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
        {
            std::map<String, VariantPropertySpec> in, out;
            for (auto &step : stepList) {
                step->extractInputVariableNames(in);
                for (const auto &nameSpecP : out) {
                    in.erase(nameSpecP.first);
                }
                outSpecs.insert(in.begin(), in.end());
                in.clear();
                step->extractOutputVariableNames(out);
            }
        }

        void extractOutputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
        {
            if (!enablingCondition) {
                for (auto &step : stepList) {
                    step->extractOutputVariableNames(outSpecs);
                }
            }
        }
        
        void extractPrimitiveSteps(std::vector<const PrimitiveStepT*> &outSteps, const std::map<String, VariantProperty> &properties) const
        {
            if (!enablingCondition || enablingCondition(properties)) {
                for (auto &step : stepList) {
                    step->extractPrimitiveSteps(outSteps, properties);
                }
            }
        } 


        std::vector<casted_resource_ptr<BaseStepT>> stepList;
        std::function<bool(const std::map<String, VariantProperty> &properties)> enablingCondition;
    };
    
}