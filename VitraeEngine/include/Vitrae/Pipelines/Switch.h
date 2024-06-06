#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "dynasma/pointer.hpp"

namespace Vitrae
{
template <TaskChild BasicTask> class Switch : public BasicTask
{
  public:
    using SwitchEnumType = std::size_t;

  protected:
    std::map<SwitchEnumType, dynasma::FirmPtr<BasicTask>> m_taskMap;
    StringId m_selectionProperty;
    PropertySpec m_selectionPropertySpec;

    void updateInputOutputProperties()
    {
        this->m_inputSpecs.clear();
        this->m_outputSpecs.clear();

        for (auto &pair : m_taskMap) {
            this->m_inputSpecs.insert(pair.second->m_inputSpecs.begin(),
                                      pair.second->m_inputSpecs.end());
            this->m_outputSpecs.insert(pair.second->m_outputSpecs.begin(),
                                       pair.second->m_outputSpecs.end());
        }

        this->m_inputSpecs.insert({m_selectionProperty, m_selectionPropertySpec});
    }

  public:
    Switch(std::map<SwitchEnumType, dynasma::FirmPtr<BasicTask>> taskMap,
           PropertySpec selectionPropertySpec)
        : m_taskMap(taskMap), m_selectionProperty(selectionPropertySpec.name),
          m_selectionPropertySpec(selectionPropertySpec)
    {
        updateInputOutputProperties();
    }

    ~Switch() = default;

    /**
     * Adds an option to the task map and updates the input/output properties.
     *
     * @param property The property to associate with the task.
     * @param task The task to be associated with the property.
     */
    void addOption(SwitchEnumType value, dynasma::FirmPtr<BasicTask> task)
    {

        m_taskMap.insert({value, task});
        updateInputOutputProperties();
    }

    /**
     * Removes the specified option from the task map and updates the
     * input/output properties.
     *
     * @param property the property to be removed
     */
    void removeOption(SwitchEnumType value)
    {

        m_taskMap.erase(value);
        updateInputOutputProperties();
    }

    /**
     * Retrieves a task associated with the given property.
     *
     * @param property the property used to look up the task
     *
     * @return a pointer to the task associated with the property
     *
     * @throws std::out_of_range if the property is not found in the task map
     */
    dynasma::FirmPtr<BasicTask> getTask(SwitchEnumType value) { return m_taskMap.at(value); }

    void extractUsedTypes(std::set<const TypeInfo *> &typeSet) const override
    {
        for (auto [id, task] : m_taskMap) {
            task->extractUsedTypes(typeSet);
        }
    }

    void extractSubTasks(std::set<dynasma::LazyPtr<Task>> &taskSet) const override
    {
        for (auto [id, task] : m_taskMap) {
            taskSet.insert(task);
        }
    }
};
} // namespace Vitrae