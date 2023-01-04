#pragma once

#include <memory>
#include <string>

namespace Vitrae
{
    template<class _T>
    using Unique = std::unique_ptr<_T>;

    template<class _T>
    using Shared = std::shared_ptr<_T>;

    template<class _T>
    using Pointer = _T*;

    using String = std::string;
}