#pragma once

#include "Vitrae/Assets/Mesh.hpp"
#include "Vitrae/Types/Transformation.hpp"

#include "assimp/scene.h"
#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"

#include <filesystem>

namespace Vitrae
{
class ComponentRoot;

class Camera : public SimpleTransformation
{
  public:
    float zNear = 1.0, zFar = 1000.0;
    float width, height;
    float fov = 45.0;

    inline glm::mat4 getPerspectiveMatrix() const
    {
        return glm::perspective(glm::radians(fov), width / height, zNear, zFar);
    }
};

struct MeshProp
{
    dynasma::FirmPtr<Mesh> p_mesh;
    SimpleTransformation transform;
};

/**
 * Represents a scene
 */
class Scene : public dynasma::PolymorphicBase
{
  public:
    struct AssimpLoadParams
    {
        const aiScene *p_extScene;
        ComponentRoot &root;
    };
    struct FileLoadParams
    {
        std::filesystem::path filepath;
        ComponentRoot &root;
    };

    Scene(const AssimpLoadParams &params);
    Scene(const FileLoadParams &params);

    /*
    Scene parts (to be replaced with a more modular approach)
    */

    std::vector<MeshProp> meshes;
    Camera camera;

  protected:
    void loadFromAssimp(const AssimpLoadParams &params);
};

} // namespace Vitrae