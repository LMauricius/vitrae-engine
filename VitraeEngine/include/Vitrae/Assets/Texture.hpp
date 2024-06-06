#pragma once

#include "dynasma/core_concepts.hpp"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"

#include <filesystem>

namespace Vitrae
{
class ComponentRoot;

/**
 * A Texture is a single image-like resource
 */
class Texture : public dynasma::PolymorphicBase
{
  public:
    struct FileLoadParams
    {
        std::filesystem::path filepath;
        ComponentRoot &root;
    };

    virtual std::size_t memory_cost() const = 0;

    virtual ~Texture() = 0;
};

struct TextureSeed
{
    using Asset = Texture;

    inline std::size_t load_cost() const { return 1; }

    std::variant<Texture::FileLoadParams> kernel;
};

using TextureManager = dynasma::AbstractManager<TextureSeed>;
// using TextureKeeper = dynasma::AbstractKeeper<ImmediateTextureSeed>;
} // namespace Vitrae