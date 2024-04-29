#include "Vitrae/Util/StringId.h"

namespace Vitrae
{

    StringId::StringId(const std::string &str)
    {
        m_hash = std::hash<std::string>{}(str);
    }

    StringId::StringId(const std::string_view str)
    {
        m_hash = std::hash<std::string_view>{}(str);
    }

    bool StringId::operator=(StringId id)
    {
        m_hash = id.m_hash;
        return true;
    }

    bool StringId::operator==(StringId id) const
    {
        return m_hash == id.m_hash;
    }

    bool StringId::operator!=(StringId id) const
    {
        return m_hash != id.m_hash;
    }

    bool StringId::operator>=(StringId id) const
    {
        return m_hash >= id.m_hash;
    }

    bool StringId::operator<=(StringId id) const
    {
        return m_hash <= id.m_hash;
    }

    bool StringId::operator>(StringId id) const
    {
        return m_hash > id.m_hash;
    }

    bool StringId::operator<(StringId id) const
    {
        return m_hash < id.m_hash;
    }

}