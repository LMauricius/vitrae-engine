#pragma once

#include "Vitrae/Pipelines/Task.h"
#include "dynasma/pointer.hpp"

#include <vector>
namespace Vitrae
{

template <TaskChild BasicTask> class Group : public BasicTask
{
  public:
    struct GroupItemEntry
    {
        dynasma::FirmPtr<BasicTask> task;
        std::map<StringId, StringId> inputParamsToSharedVariables;
        std::map<StringId, StringId> outputParamsToSharedVariables;
    };

  protected:
    std::vector<GroupItemEntry> m_items;
    std::map<StringId, PropertySpec> m_localSpecs;

  private:
    void updateInputOutputProperties()
    {
        this->m_inputSpecs.clear();
        this->m_outputSpecs.clear();

        for (auto &item : m_items)
        {
            for (auto [inputName, spec] : item.task->m_inputSpecs)
            {
                StringId localName = item.m_inputParamsToSharedVariables.at(inputName);
                auto i_localSpec = m_localSpecs.find(localName);

                if (i_localSpec == m_localSpecs.end())
                {
                    this->m_inputSpecs.insert({localName, spec});
                }
            }

            for (auto [outputName, spec] : item.task->m_outputSpecs)
            {
                StringId localName = item.m_outputParamsToSharedVariables.at(outputName);
                auto i_localSpec = m_localSpecs.find(localName);

                if (i_localSpec == m_localSpecs.end())
                {
                    m_localSpecs.insert({localName, spec});
                }
            }
        }
    }

  public:
    Group() = default;
    Group(const std::map<StringId, PropertySpec> &outputSpecs,
          const std::vector<GroupItemEntry> &items)
        : m_items(items)
    {
        this->m_outputSpecs = outputSpecs;
        updateInputOutputProperties();
    }
    Group(std::map<StringId, PropertySpec> &&outputSpecs, std::vector<GroupItemEntry> &&items)
        : m_items(std::move(items))
    {
        this->m_outputSpecs = std::move(outputSpecs);
        updateInputOutputProperties();
    }

    ~Group() = default;
};
} // namespace Vitrae