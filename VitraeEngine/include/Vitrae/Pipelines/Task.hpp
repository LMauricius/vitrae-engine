#pragma once

#include "Vitrae/Types/Typedefs.hpp"
#include "Vitrae/Util/PropertyList.hpp"
#include "Vitrae/Util/StringId.hpp"
#include "Vitrae/Util/Variant.hpp"

#include "dynasma/pointer.hpp"
#include "dynasma/util/dynamic_typing.hpp"

#include <map>
#include <set>
#include <span>

namespace Vitrae
{

class Task : public dynasma::PolymorphicBase
{
  protected:
    std::map<StringId, PropertySpec> m_inputSpecs;
    std::map<StringId, PropertySpec> m_outputSpecs;

  public:
    Task() = delete;
    Task(Task const &) = default;
    Task(Task &&) = default;

    template <class ContainerT>
    inline Task(const ContainerT &inputSpecs, const ContainerT &outputSpecs)
        requires(std::ranges::range<ContainerT> &&
                 std::convertible_to<std::ranges::range_value_t<ContainerT>, const PropertySpec &>)
    {
        for (const auto &spec : inputSpecs) {
            m_inputSpecs.emplace(spec.name, spec);
        }
        for (const auto &spec : outputSpecs) {
            m_outputSpecs.emplace(spec.name, spec);
        }
    }
    inline Task(const std::map<StringId, PropertySpec> &inputSpecs,
                const std::map<StringId, PropertySpec> &outputSpecs)
        : m_inputSpecs(inputSpecs), m_outputSpecs(outputSpecs)
    {}
    inline Task(std::map<StringId, PropertySpec> &&inputSpecs,
                std::map<StringId, PropertySpec> &&outputSpecs)
        : m_inputSpecs(std::move(inputSpecs)), m_outputSpecs(std::move(outputSpecs))
    {}
    virtual ~Task() = default;

    Task &operator=(Task const &) = default;
    Task &operator=(Task &&) = default;

    virtual std::size_t memory_cost() const = 0;

    inline std::map<StringId, PropertySpec> &getInputSpecs() { return m_inputSpecs; }
    inline std::map<StringId, PropertySpec> &getOutputSpecs() { return m_outputSpecs; }
    virtual void extractUsedTypes(std::set<const TypeInfo *> &typeSet) const = 0;
    virtual void extractSubTasks(std::set<const Task *> &taskSet) const = 0;
};

template <class T>
concept TaskChild = std::is_base_of_v<Task, T>;

} // namespace Vitrae