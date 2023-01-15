#pragma once

#include "Vitrae/Util/Types.h"

#include "assimp/types.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include <type_traits>
#include <utility>
#include <sstream>

namespace Vitrae
{
    template<class T> requires requires(T t, std::stringstream ss) {ss << t;}
    String toString(const T& val)
    {
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    template<class T> requires requires(T t) {glm::to_string(t);}
    String toString(const T& val)
    {
        return glm::to_string(val);
    }

    template<class T> requires requires(T t) {std::to_string(t);}
    String toString(const T& val)
    {
        return std::to_string(val);
    }

    String toString(const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(str);
    }
    String toString(const aiString& str)
    {
        return String(str.data, str.length);
    }
}