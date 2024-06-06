#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "dynasma/pointer.hpp"

namespace Vitrae
{

template <TaskChild BasicTask> class Filter : public BasicTask
{
  protected:
    dynasma::FirmPtr<BasicTask> m_task;
    StringId m_conditionProperty;
    PropertySpec m_conditionPropertySpec;

  public:
    Filter(dynasma::FirmPtr<BasicTask> containedTask, PropertySpec conditionPropertySpec)
        : m_task(containedTask), m_conditionProperty(conditionPropertySpec.name),
          m_conditionPropertySpec(conditionPropertySpec)
    {
        this->m_inputSpecs = m_task->m_inputSpecs;
        this->m_outputSpecs = m_task->m_outputSpecs;

        if (this->m_inputSpecs != this->m_outputSpecs) {
            throw std::runtime_error("Input and output specs must be equal");
        }

        this->m_inputSpecs.insert({m_conditionProperty, m_conditionPropertySpec});
    }

    ~Filter() = default;

    void extractUsedTypes(std::set<const TypeInfo *> &typeSet) const override
    {
        m_task->extractUsedTypes(typeSet);
    }

    void extractSubTasks(std::set<dynasma::LazyPtr<Task>> &taskSet) const override
    {
        taskSet.insert(m_task);
    }
};

} // namespace Vitrae