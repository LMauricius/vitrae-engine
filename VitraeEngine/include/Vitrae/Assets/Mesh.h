#pragma once

#include "Vitrae/Types/GraphicPrimitives.h"
#include "Vitrae/Assets/Material.h"
#include "Vitrae/Util/NonCopyable.h"

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "dynasma/pointer.hpp"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"

#include <span>
#include <variant>

namespace Vitrae
{
    class ComponentRoot;
    class Material;

    /**
     * A mesh is a 3D polygonal piece of geometry,
     * with an assigned Material
     */
    class Mesh : public dynasma::PolymorphicBase
    {
    public:
        struct AssimpLoadParams
        {
            const aiMesh &extMesh;
            ComponentRoot &root;
        };

        virtual ~Mesh() = 0;

        virtual void setMaterial(dynasma::LazyPtr<Material> mat) = 0;
        virtual dynasma::LazyPtr<Material> getMaterial() const = 0;
        virtual std::span<const Triangle> getTriangles() const = 0;

        virtual std::size_t memory_cost() const = 0;
    };

    struct ImmediateMeshSeed
    {
        using Asset = Mesh;

        std::variant<Mesh::AssimpLoadParams> kernel;
        [[no_unique_address]] NonCopyable _ = {};

        inline std::size_t load_cost() const { return 1; }
    };

    /**
     * Namespace containing all standard vertex buffer names
     */
    namespace StandardVertexBufferNames
    {
    constexpr const char POSITION[] = "position";
    constexpr const char NORMAL[] = "normal";
    constexpr const char TEXTURE_COORD[] = "textureCoord0";
    constexpr const char COLOR[] = "color0";
    } // namespace StandardVertexBufferNames

    // using MeshManager = dynasma::AbstractManager<MeshSeed>;
    using MeshKeeper = dynasma::AbstractKeeper<ImmediateMeshSeed>;
}