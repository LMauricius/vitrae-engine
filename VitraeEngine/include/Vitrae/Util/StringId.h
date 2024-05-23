#pragma once

#include <string>

namespace Vitrae
{
/**
 * A hash of a string, for quicker comparison and mapping
 */
class StringId
{
    std::size_t m_hash;

  public:
    StringId(const char *str);
    StringId(const std::string &str);
    StringId(const std::string_view str);
    StringId(StringId &&) = default;
    StringId(const StringId &) = default;

    bool operator=(StringId id);

    bool operator==(StringId id) const;
    bool operator!=(StringId id) const;
    bool operator>=(StringId id) const;
    bool operator<=(StringId id) const;
    bool operator>(StringId id) const;
    bool operator<(StringId id) const;
};
} // namespace Vitrae