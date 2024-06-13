#include "Vitrae/Visuals/Scene.hpp"

#include "Vitrae/ComponentRoot.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Vitrae
{
Scene::Scene(const AssimpLoadParams &params)
{
    loadFromAssimp(params);
}

Scene::Scene(const FileLoadParams &params)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(params.filepath.c_str(),
                                             aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType | aiProcess_FlipUVs);

    if (!scene) {
        params.root.getErrStream() << importer.GetErrorString();
        return;
    }

    loadFromAssimp({scene, params.root});

    importer.FreeScene();
}

void Scene::loadFromAssimp(const AssimpLoadParams &params)
{
    MeshKeeper &meshKeeper = params.root.getComponent<MeshKeeper>();

    if (params.p_extScene->HasMeshes()) {
        for (unsigned int i = 0; i < params.p_extScene->mNumMeshes; ++i) {
            dynasma::FirmPtr<Mesh> p_mesh = meshKeeper.new_asset(
                {Mesh::AssimpLoadParams{params.p_extScene->mMeshes[i], params.root}});

            meshProps.emplace_back(MeshProp{p_mesh, SimpleTransformation()});
        }
    }

    if (params.p_extScene->HasCameras()) {
        const aiCamera *p_ext_camera = params.p_extScene->mCameras[0];

        camera.position = glm::vec3(p_ext_camera->mPosition.x, p_ext_camera->mPosition.y,
                                    p_ext_camera->mPosition.z);
        camera.rotation = glm::lookAt(camera.position,
                                      camera.position + glm::vec3(p_ext_camera->mLookAt.x,
                                                                  p_ext_camera->mLookAt.y,
                                                                  p_ext_camera->mLookAt.z),
                                      glm::vec3(0, 1, 0));
    }
}

} // namespace Vitrae