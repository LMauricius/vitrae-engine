#include "Vitrae/Util/ScopedDict.hpp"

#include <stdexcept>

namespace Vitrae
{

ScopedDict::ScopedDict() : m_parent{nullptr}
{
}
ScopedDict::ScopedDict(const ScopedDict *parent) : m_parent{parent}
{
}

void ScopedDict::set(StringId key, const Property &value)
{
    m_dict[key] = value;
}

void ScopedDict::set(StringId key, Property &&value)
{
    m_dict[key] = std::move(value);
}

const Property &ScopedDict::get(StringId key) const
{
    auto it = m_dict.find(key);
    if (it != m_dict.end())
        return it->second;

    if (m_parent)
        return m_parent->get(key);

    throw std::runtime_error{"Key not found: " + std::string(key)};
}

bool ScopedDict::has(StringId key) const
{
    return m_dict.find(key) != m_dict.end() || (m_parent && m_parent->has(key));
}

} // namespace Vitrae