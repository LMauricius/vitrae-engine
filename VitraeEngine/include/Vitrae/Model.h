#pragma once

#include "ResourceManager.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include <vector>
#include <filesystem>

namespace Vitrae
{
    class ResourceRoot;
    class Mesh;

    /**
     * A mesh is a 3D polygonal piece of geometry,
     * with an assigned Material
    */
    class Model
    {
    public:
        struct SetupParams {};
        struct LoadParams {
            std::filesystem::path filepath;
            ResourceRoot &resRoot;
        };
        
        virtual ~Model();

        virtual void load(const LoadParams& params);

        inline const std::vector<resource_ptr<Mesh>>& getMeshes() const {return mMeshes;}

    protected:
        std::vector<resource_ptr<Mesh>> mMeshes;
    };
    
}