#include "shadingOverrider.hpp"

#include "Vitrae/Assets/Mesh.hpp"
#include "Vitrae/Visuals/Scene.hpp"

#include <set>

void ShadingOverrider::overrideSceneMaterials(ComponentRoot &root, Compositor &compositor)
{
    Scene &scene = *compositor.parameters.get("scene").get<dynasma::FirmPtr<Scene>>();

    // extract materials
    std::set<dynasma::FirmPtr<Material>> materials;
    for (auto &prop : scene.meshProps) {
        materials.insert(prop.p_mesh->getMaterial());
    }

    // apply our methods
    for (auto p_mat : materials) {
        p_mat->setMethods(p_vertexMethod, p_fragmentMethod);
    }

    compositor.setComposeMethod(p_composeMethod);

    // override defaults
    root.addAiMaterialShadingInfo(aiShadingMode_Phong, Vitrae::ComponentRoot::AiMaterialShadingInfo{
                                                           .vertexMethod = p_vertexMethod,
                                                           .fragmentMethod = p_fragmentMethod});
}