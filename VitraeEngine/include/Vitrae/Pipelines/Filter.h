#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "dynasma/pointer.hpp"

namespace Vitrae {
template<TaskChild BasicTask>
class Filter : public BasicTask
{
  public:
    using FilterConditionType = bool;

  protected:
    dynasma::FirmPtr<BasicTask> m_task;
    StringId m_conditionProperty;

  public:
    Filter(dynasma::FirmPtr<BasicTask> containedTask, StringId conditionProperty)
        : m_task(containedTask), m_conditionProperty(conditionProperty)
    {
        this->m_inputSpecs = m_task->m_inputSpecs;
        this->m_outputSpecs = m_task->m_outputSpecs;

        if (this->m_inputSpecs != this->m_outputSpecs)
        {
            throw std::runtime_error("Input and output specs must be equal");
        }

        this->m_inputSpecs.insert({m_conditionProperty, typeid(bool)});
  }

  ~Filter() = default;
};
} // namespace Vitrae