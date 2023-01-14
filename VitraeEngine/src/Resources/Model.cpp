#include "Vitrae/Resources/Model.h"
#include "Vitrae/ResourceRoot.h"
#include "Vitrae/Resources/Mesh.h"
#include "Vitrae/Resources/Material.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Vitrae
{

    Model::~Model()
    {

    }

    void Model::load(const LoadParams& params)
    {
		Assimp::Importer importer;

		const aiScene* extScenePtr = importer.ReadFile(params.filepath.c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType | aiProcess_FlipUVs);

		if (!extScenePtr) {
			params.resRoot.getErrStream() << "Loading scene failed: " << importer.GetErrorString() << std::endl;
			return;
		}

        // load materials
        if (extScenePtr->HasMaterials()) {
            for (int i=0; i<extScenePtr->mNumMaterials; i++) {
                params.resRoot.getManager<Material>().createResource(
                    toString(extScenePtr->mMaterials[i]->GetName()),
                    Material::SetupParams{
                        *(extScenePtr->mMaterials[i]),
                        params.resRoot
                    },
                    Material::LoadParams()
                );
            }
        }

        // load meshes
        if (extScenePtr->HasMeshes()) {
            for (int i=0; i<extScenePtr->mNumMeshes; i++) {

                auto mesh = params.resRoot.getManager<Mesh>().createResource(
                    toString(extScenePtr->mMeshes[i]->mName),
                    Mesh::SetupParams{
                        *(extScenePtr->mMeshes[i]),
                        params.resRoot
                    },
                    Mesh::LoadParams()
                );

                // set material
                auto extMaterialPtr = extScenePtr->mMaterials[extScenePtr->mMeshes[i]->mMaterialIndex];
                if (extMaterialPtr) {
                    String materialName = toString(extMaterialPtr->GetName());
                    mesh->setMaterial(params.resRoot.getManager<Material>().getResource(materialName));
                }

                mMeshes.push_back(mesh);
            }
        }
    }
}