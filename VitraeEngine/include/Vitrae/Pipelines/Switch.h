#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/AssetManager.h"

#include <span>
#include <map>
#include <functional>
#include <optional>

namespace Vitrae
{
    template<class BaseStepT, class PrimitiveStepT>
    class GenericSwitchStep: public BaseStepT
    {
    public:
        struct SetupParams {};
        struct LoadParams {};

        struct Case
        {
            casted_resource_ptr<ShaderStep> choice;
            std::function<bool(const std::map<String, VariantProperty> &properties)> enablingCondition;
        };

        GenericSwitchStep() = default;
        ~GenericSwitchStep() = default;

        void extractInputPropertyNames(std::set<String> &outNames) const
        {
            for (auto &c : cases) {
                c.choice->extractInputPropertyNames(outNames);
            }
            if (defaultChoice.has_value()) {
                defaultChoice.value()->extractInputPropertyNames(outNames);
            }
        }

        void extractInputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
        {
            for (auto &c : cases) {
                c.choice->extractInputVariableNames(outSpecs);
            }
            if (defaultChoice.has_value()) {
                defaultChoice.value()->extractInputVariableNames(outSpecs);
            }
        }
        
        void extractOutputVariableNames(std::map<String, VariantPropertySpec> &outSpecs) const
        {
            if (defaultChoice.has_value()) {
                std::map<String, VariantPropertySpec> interOut, out;
                defaultChoice.value()->extractOutputVariableNames(interOut);

                for (auto &c : cases) {
                    c.choice->extractOutputVariableNames(out);
                    for (auto it = interOut.begin(); it != interOut.end();) {
                        if (out.find(it->first) == out.end()) {
                            interOut.erase(it++);
                        }
                        else {
                            it++;
                        }
                    }
                    out.clear();
                }
            }
        }
        
        void extractPrimitiveSteps(std::vector<const PrimitiveStepT*> &outSteps, const std::map<String, VariantProperty> &properties) const
        {
            for (auto &c : cases) {
                if (!c.enablingCondition || c.enablingCondition(properties)) {
                    c.choice->extractPrimitiveSteps(outSteps, properties);
                    return;
                }
            }
            if (defaultChoice.has_value()) {
                defaultChoice.value()->extractPrimitiveSteps(outSteps, properties);
            }
        }   


        std::vector<Case> cases;
        std::optional<casted_resource_ptr<BaseStepT>> defaultChoice;
    };
    
}