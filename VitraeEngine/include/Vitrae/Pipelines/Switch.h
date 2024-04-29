#pragma once

#include "Vitrae/Pipelines/Task.h"

namespace Vitrae {
template<TaskChild BasicTask>
class Switch : public BasicTask
{
protected:
  std::map<Property, dynasma::FirmPtr<BasicTask>> m_taskMap;
  std::pair<StringId, PropertySpec> m_selectionProperty;

  void updateInputOutputProperties()
  {

    this->m_inputSpecs.clear();
    this->m_outputSpecs.clear();

    if (!m_taskMap.empty()) {
      this->m_outputSpecs = m_taskMap.begin()->second->m_outputSpecs;
    }

    for (auto& pair : m_taskMap) {
      this->m_inputSpecs.insert(pair.second->m_inputSpecs.begin(), pair.second->m_inputSpecs.end());

      for (auto it = this->m_outputSpecs.begin(); it != this->m_outputSpecs.end();) {
        if (pair.second->m_outputSpecs.find(it->first) == pair.second->m_outputSpecs.end()) {
          this->m_outputSpecs.erase(it++);
        } else {
          ++it;
        }
      }
    }

    m_inputSpecs.insert(m_selectionProperty);
  }

public:
  Switch(std::map<Property, dynasma::FirmPtr<BasicTask>> taskMap,
         std::pair<StringId, PropertySpec> selectionProperty)
    : m_taskMap(taskMap)
    , m_selectionProperty(selectionProperty)
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
  void addOption(Property property, dynasma::FirmPtr<BasicTask> task)
  {

    m_taskMap.insert({ property, task });
    updateInputOutputProperties();
  }

  /**
   * Removes the specified option from the task map and updates the
   * input/output properties.
   *
   * @param property the property to be removed
   */
  void removeOption(Property property)
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
  dynasma::FirmPtr<BasicTask> getTask(Property property) { return m_taskMap.at(property); }
};
} // namespace Vitrae