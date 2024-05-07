#pragma once

#include "Vitrae/Util/Property.h"

#include <map>

namespace Vitrae
{

class ScopedDict
{
    const ScopedDict *m_parent;
    std::map<StringID, Property> m_dict;

  public:
    ScopedDict();
    ScopedDict(const ScopedDict *parent);

    void set(StringID key, const Property &value);
    void set(StringID key, Property &&value);
    const Property &get(StringID key) const;
    bool has(StringID key) const;
};

} // namespace Vitrae