#pragma once

#include "Vitrae/Util/NonCopyable.h"

#include "dynasma/pointer.hpp"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"
#include "assimp/material.h"

#include <variant>

namespace Vitrae
{
    class ComponentRoot;
    class Texture;

    class Material : public dynasma::PolymorphicBase
    {
    public:
        struct AssimpLoadParams
        {
            const aiMaterial *p_extMaterial;
            ComponentRoot &root;
        };

        Material(const AssimpLoadParams &params);
        virtual ~Material();

        std::size_t memory_cost() const;
    };

    struct ImmediateMaterialSeed
    {
        using Asset = Material;

        std::variant<Material::AssimpLoadParams> kernel;

        inline std::size_t load_cost() const { return 1; }
    };

    // using MaterialManager = dynasma::AbstractManager<MaterialSeed>;
    using MaterialKeeper = dynasma::AbstractKeeper<ImmediateMaterialSeed>;
}