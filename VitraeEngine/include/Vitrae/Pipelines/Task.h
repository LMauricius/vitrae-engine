#pragma once

#include "Vitrae/Types/Typedefs.h"
#include "Vitrae/Util/Property.h"
#include "Vitrae/Util/StringId.h"

#include "dynasma/util/dynamic_typing.hpp"

#include <map>
#include <span>

namespace Vitrae
{

class Task : public dynasma::PolymorphicBase
{
  protected:
    std::map<StringId, PropertySpec> m_inputSpecs;
    std::map<StringId, PropertySpec> m_outputSpecs;

  public:
    virtual ~Task() = default;

    virtual std::size_t memory_cost() const = 0;

    inline std::map<StringId, PropertySpec> &getInputSpecs()
    {
        return m_inputSpecs;
    }
    inline std::map<StringId, PropertySpec> &getOutputSpecs()
    {
        return m_outputSpecs;
    }
};

template <class T>
concept TaskChild = std::is_base_of_v<Task, T>;

} // namespace Vitrae