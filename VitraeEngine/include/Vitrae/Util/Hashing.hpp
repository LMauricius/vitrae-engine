#pragma once

#include <cstdint>
#include <span>

namespace Vitrae
{

template <std::size_t N> inline std::size_t combinedHashes(std::span<const std::size_t, N> hashes)
{
    std::size_t seed = 0;

    for (std::size_t hash : *hashes) {
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    return seed;
}

} // namespace Vitrae