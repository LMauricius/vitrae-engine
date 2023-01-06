#pragma once

#include <memory>
#include <string>
#include <codecvt>
#include <locale>
#include "assimp/types.h"

namespace Vitrae
{
    template<class _T>
    using Unique = std::unique_ptr<_T>;

    template<class _T>
    using Shared = std::shared_ptr<_T>;

    template<class _T>
    using Pointer = _T*;

    using String = std::string;
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