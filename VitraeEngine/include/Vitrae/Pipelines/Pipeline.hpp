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
    Pipeline(dynasma::FirmPtr<Method<BasicTask>> method, std::span<PropertySpec> desiredOutputSpecs)
    {
        // store outputs
        for (auto &outputSpec : desiredOutputSpecs) {
            outputSpecs[outputSpec.name] = outputSpec;
        }

        // solve dependencies
        std::set<dynasma::FirmPtr<BasicTask>> visitedTasks;
        std::map<StringId, bool> visitedOutputs;

        for (auto &outputSpec : desiredOutputSpecs) {
            tryAddDependency(outputSpec);
        }
    }

    std::map<StringId, PropertySpec> localSpecs, inputSpecs, outputSpecs;
    std::vector<PipeItem> items;

  protected:
    bool tryAddDependency(std::set<StringId> &visitedOutputs, const PropertySpec &desiredOutputSpec,
                          bool isPipelineOutput)
    {
        if (visitedOutputs.find(desiredOutputSpec.name) == visitedOutputs.end()) {
            std::optional<dynasma::FirmPtr<BasicTask>> task = method->getTask(outputNameId);

            if (task.has_value()) {

                // task inputs (also handle deps)
                std::map<StringId, StringId> inputToLocalVariables;
                for (auto &inputSpec : task.value()->getInputSpecs()) {
                    tryAddDependency(desiredOutputSpec);
                    inputToLocalVariables.emplace(inputSpec.name, inputSpec.name);
                }

                // task outputs (store the outputs as visited)
                std::map<StringId, StringId> outputToLocalVariables;
                for (auto &taskOutputSpec : task.value()->getOutputSpecs()) {
                    if (outputSpecs.find(taskOutputSpec.name) == outputSpecs.end() &&
                        localSpecs.find(taskOutputSpec.name) == localSpecs.end()) {
                        localSpecs[taskOutputSpec.name] = taskOutputSpec;

                        visitedOutputs.insert(taskOutputSpec.name);
                    }
                    outputToLocalVariables.emplace(taskOutputSpec.name, taskOutputSpec.name);
                }

                items.emplace_back({task.value(), inputToLocalVariables, outputToLocalVariables});
            } else {
                inputSpecs[desiredOutputSpec.name] = desiredOutputSpec;

                visitedOutputs.insert(desiredOutputSpec.name);
            }
        }
    };
};
} // namespace Vitrae