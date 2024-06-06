#pragma once

#include <string>
#include <string_view>

namespace Vitrae
{
/**
 * A hash of a string, for quicker comparison and mapping
 */
class StringId
{
    friend std::hash<StringId>;

    std::size_t m_hash;

  public:
    constexpr StringId(const char *str) : StringId(std::string_view{str}) {}
    constexpr StringId(const std::string &str) : StringId(std::string_view{str}) {}
    constexpr StringId(const std::string_view str)
    {
        // fnv_hash_1a_64
        m_hash = 0xcbf29ce484222325ULL;

        for (char c : str)
            m_hash = (m_hash ^ c) * 0x100000001b3ULL;
    }
    constexpr StringId(StringId &&) = default;
    constexpr StringId(const StringId &) = default;

    constexpr StringId &operator=(StringId id)
    {
        m_hash = id.m_hash;
        return *this;
    }

    constexpr bool operator==(StringId id) const { return m_hash == id.m_hash; }
    constexpr bool operator!=(StringId id) const { return m_hash != id.m_hash; }
    constexpr bool operator>=(StringId id) const { return m_hash >= id.m_hash; }
    constexpr bool operator<=(StringId id) const { return m_hash <= id.m_hash; }
    constexpr bool operator>(StringId id) const { return m_hash > id.m_hash; }
    constexpr bool operator<(StringId id) const { return m_hash < id.m_hash; }
};
} // namespace Vitrae

namespace std
{
template <> struct hash<Vitrae::StringId>
{
    size_t operator()(const Vitrae::StringId &x) const { return x.m_hash; }
};
} // namespace std