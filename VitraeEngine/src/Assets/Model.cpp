#include "Vitrae/Assets/Model.h"
#include "Vitrae/ComponentRoot.h"
#include "Vitrae/Assets/Mesh.h"
#include "Vitrae/Assets/Material.h"
#include "Vitrae/Util/StringConvert.h"

#include "dynasma/keepers/abstract.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Vitrae
{
    Model::Model(const FileLoadParams &params)
    {
        const std::filesystem::path &filepath = params.filepath;
        ComponentRoot &root = params.root;

        // prepare needed managers
        MeshKeeper &meshKeeper = root.getComponent<MeshKeeper>();
        MaterialKeeper &matKeeper = root.getComponent<MaterialKeeper>();

        // load scene
        Assimp::Importer importer;

        const aiScene *extScenePtr = importer.ReadFile(params.filepath.c_str(),
                                                       aiProcess_CalcTangentSpace |
                                                           aiProcess_Triangulate |
                                                           aiProcess_JoinIdenticalVertices |
                                                           aiProcess_SortByPType | aiProcess_FlipUVs);

        if (!extScenePtr)
        {
            params.root.getErrStream() << "Loading scene failed: " << importer.GetErrorString() << std::endl;
            return;
        }

        // load materials
        std::vector<dynasma::LazyPtr<Material>> matById;
        if (extScenePtr->HasMaterials())
        {
            matById.reserve(extScenePtr->mNumMaterials);
            for (int i = 0; i < extScenePtr->mNumMaterials; i++)
            {
                auto p_mat = matKeeper.new_asset({
                    Material::AssimpLoadParams{
                        *(extScenePtr->mMaterials[i]),
                        params.root},
                });
                matById.emplace_back(p_mat);
            }
        }

        // load meshes
        if (extScenePtr->HasMeshes())
        {
            for (int i = 0; i < extScenePtr->mNumMeshes; i++)
            {

                auto p_mesh = meshKeeper.new_asset({
                                                       Mesh::AssimpLoadParams{
                                                           *(extScenePtr->mMeshes[i]),
                                                           params.root},
                                                   })
                                  .getLoaded();

                // set material
                p_mesh->setMaterial(matById[extScenePtr->mMeshes[i]->mMaterialIndex]);

                m_meshes.push_back(p_mesh);
            }
        }
    }

    Model::~Model()
    {
    }
}