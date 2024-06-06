#pragma once

#include "Vitrae/Types/Typedefs.hpp"
#include "Vitrae/Util/Property.hpp"
#include "Vitrae/Util/StringId.hpp"

#include "dynasma/pointer.hpp"
#include "dynasma/util/dynamic_typing.hpp"

#include <map>
#include <set>
#include <span>

namespace Vitrae
{

struct PropertySpec
{
    std::string name;
    const TypeInfo &typeInfo;
};

class Task : public dynasma::PolymorphicBase
{
  protected:
    std::map<StringId, PropertySpec> m_inputSpecs;
    std::map<StringId, PropertySpec> m_outputSpecs;

  public:
    virtual ~Task() = default;

    virtual std::size_t memory_cost() const = 0;

    inline std::map<StringId, PropertySpec> &getInputSpecs() { return m_inputSpecs; }
    inline std::map<StringId, PropertySpec> &getOutputSpecs() { return m_outputSpecs; }
    virtual void extractUsedTypes(std::set<const TypeInfo *> &typeSet) const = 0;
    virtual void extractSubTasks(std::set<dynasma::LazyPtr<Task>> &taskSet) const = 0;
};

template <class T>
concept TaskChild = std::is_base_of_v<Task, T>;

} // namespace Vitrae