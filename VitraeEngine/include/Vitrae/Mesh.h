#pragma once

#include "Vitrae/Util/GraphicPrimitives.h"
#include "ResourceManager.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <vector>

namespace Vitrae
{
    class ResourceRoot;
    class Material;

    /**
     * A mesh is a 3D polygonal piece of geometry,
     * with an assigned Material
    */
    class Mesh
    {
    public:
        struct SetupParams
        {
            const aiMesh& extMesh;
            ResourceRoot &resRoot;
        };
        struct LoadParams {};
        
        virtual ~Mesh() = 0;

        virtual void setMaterial(resource_ptr<Material> mat) = 0;
        virtual resource_ptr<Material> getMaterial() const = 0;
        virtual const std::vector<Vertex> &getVertices() const = 0;
        virtual const std::vector<Triangle> &getTriangles() const = 0;
    };
    
}