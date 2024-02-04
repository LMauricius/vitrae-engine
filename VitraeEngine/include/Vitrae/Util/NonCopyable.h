#pragma once

namespace Vitrae
{
    struct NonCopyable
    {
        NonCopyable() = default;
        NonCopyable(NonCopyable &&) = default;
        NonCopyable &operator=(NonCopyable &&) = default;
    };
}