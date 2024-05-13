#pragma once

#include "Vitrae/Pipelines/Task.h"

namespace Vitrae
{
template <TaskChild BasicTask> class Switch : public BasicTask
{
  public:
    using SwitchEnumType = std::size_t;

  protected:
    std::map<SwitchEnumType, dynasma::FirmPtr<BasicTask>> m_taskMap;
    StringId m_selectionProperty;

    void updateInputOutputProperties()
    {
        this->m_inputSpecs.clear();
        this->m_outputSpecs.clear();

        for (auto &pair : m_taskMap)
        {
            this->m_inputSpecs.insert(pair.second->m_inputSpecs.begin(),
                                      pair.second->m_inputSpecs.end());
            this->m_outputSpecs.insert(pair.second->m_outputSpecs.begin(),
                                       pair.second->m_outputSpecs.end());
        }

        m_inputSpecs.insert(m_selectionProperty);
    }

  public:
    Switch(std::map<SwitchEnumType, dynasma::FirmPtr<BasicTask>> taskMap,
           StringId selectionProperty)
        : m_taskMap(taskMap), m_selectionProperty(selectionProperty)
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

        m_taskMap.erase(property);
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
    dynasma::FirmPtr<BasicTask> getTask(SwitchEnumType value)
    {
        return m_taskMap.at(value);
    }
};
} // namespace Vitrae