#pragma once

#include "Vitrae/Pipelines/Method.hpp"

#include <span>

namespace Vitrae
{

template <TaskChild BasicTask> class Pipeline
{
  public:
    struct PipeItem
    {
        dynasma::FirmPtr<BasicTask> task;
        std::map<StringId, StringId> inputToLocalVariables;
        std::map<StringId, StringId> outputToLocalVariables;
    };

    /**
     * Constructs a pipeline using the preffered method to get desired results
     * @param method The preffered method
     * @param desiredOutputNameIds The desired outputs
     */
    Pipeline(dynasma::FirmPtr<Method<BasicTask>> method,
             std::span<const PropertySpec> desiredOutputSpecs)
    {
        // store outputs
        for (auto &outputSpec : desiredOutputSpecs) {
            outputSpecs.emplace(outputSpec.name, outputSpec);
        }

        // solve dependencies
        std::set<dynasma::FirmPtr<BasicTask>> visitedTasks;
        std::set<StringId> visitedOutputs;

        for (auto &outputSpec : desiredOutputSpecs) {
            tryAddDependency(*method, visitedOutputs, outputSpec);
        }
    }

    std::map<StringId, PropertySpec> localSpecs, inputSpecs, outputSpecs;
    std::vector<PipeItem> items;

  protected:
    bool tryAddDependency(const Method<BasicTask> &method, std::set<StringId> &visitedOutputs,
                          const PropertySpec &desiredOutputSpec)
    {
        if (visitedOutputs.find(desiredOutputSpec.name) == visitedOutputs.end()) {
            std::optional<dynasma::FirmPtr<BasicTask>> task =
                method.getTask(desiredOutputSpec.name);

            if (task.has_value()) {

                // task inputs (also handle deps)
                std::map<StringId, StringId> inputToLocalVariables;
                for (auto [inputNameId, inputSpec] : task.value()->getInputSpecs()) {
                    tryAddDependency(method, visitedOutputs, desiredOutputSpec);
                    inputToLocalVariables.emplace(inputSpec.name, inputSpec.name);
                }

                // task outputs (store the outputs as visited)
                std::map<StringId, StringId> outputToLocalVariables;
                for (auto [taskOutputNameId, taskOutputSpec] : task.value()->getOutputSpecs()) {
                    if (outputSpecs.find(taskOutputSpec.name) == outputSpecs.end() &&
                        localSpecs.find(taskOutputSpec.name) == localSpecs.end()) {
                        localSpecs.emplace(taskOutputSpec.name, taskOutputSpec);

                        visitedOutputs.insert(taskOutputSpec.name);
                    }
                    outputToLocalVariables.emplace(taskOutputSpec.name, taskOutputSpec.name);
                }

                items.push_back({task.value(), inputToLocalVariables, outputToLocalVariables});
            } else {
                inputSpecs.emplace(desiredOutputSpec.name, desiredOutputSpec.name);

                visitedOutputs.insert(desiredOutputSpec.name);
            }
        }
    };
};
} // namespace Vitrae