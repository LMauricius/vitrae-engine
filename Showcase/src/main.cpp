#include <iostream>

#include "assetCollection.hpp"
#include "classicShading.hpp"

using namespace Vitrae;

int main(int argc, char **argv)
{
    std::cout << "Hello, world!\n";

    String path = argv[1];
    float sceneScale = 1.0f;
    if (argc > 2) {
        sceneScale = std::stof(argv[2]);
    }

    AssetCollection collection(path, sceneScale);

    ClassicShading shadingModel(collection.root);
    shadingModel.overrideSceneMaterials(collection.root, collection.comp);

    /*
    Setup managers
    */

    ComponentRoot root;
    auto p_rend = new OpenGLRenderer();
    root.setComponent<Renderer>(p_rend);

    p_rend->setup(root);

    /*
    Setup window
    */

    bool running = true;
    dynasma::FirmPtr<Scene> p_scene;
    auto p_windowFrame =
        root.getComponent<FrameStoreManager>()
            .register_asset(FrameStoreSeed{FrameStore::WindowDisplayParams{
                .root = root,
                .width = 800,
                .height = 600,
                .title = "Vitrae Showcase",
                .isFullscreen = false,
                .onClose = [&]() { running = false; },
                .onDrag =
                    [&](glm::vec2 motion, bool bLeft, bool bRight, bool bMiddle) {
                        std::cout << "onDrag (" << motion.x << ", " << motion.y << ")" << std::endl;
                        if (bRight) {
                            glm::vec3 dirVec =
                                p_scene->camera.rotation * glm::vec3{0.0f, 0.0f, 1.0f};
                            float yaw = glm::orientedAngle(
                                glm::normalize(glm::vec2{dirVec.x, dirVec.z}), {0.0, 1.0});
                            float pitch = glm::orientedAngle(
                                glm::normalize(
                                    glm::vec2{glm::sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z),
                                              dirVec.y}),
                                {1.0, 0.0});
                            yaw += 0.15f * glm::radians(motion.x);
                            pitch += 0.15f * glm::radians(motion.y);
                            p_scene->camera.rotation = glm::quat(glm::vec3(pitch, yaw, 0.0f));
                        }
                        if (bLeft) {
                            p_scene->camera.move(p_scene->camera.rotation *
                                                 (0.02f * glm::vec3{-motion.x, 0.0, -motion.y}));
                        }
                        if (bMiddle) {
                            for (auto &prop : p_scene->meshProps) {
                                float scale = glm::pow(0.99f, motion.x);
                                prop.transform.position = prop.transform.position * scale;
                                prop.transform.scale(glm::vec3(scale));
                            }
                        }
                    }}})
            .getLoaded();

    /*
    Setup methods
    */

    /*
    Setup assets
    */

    p_scene = dynasma::makeStandalone<Scene>(Scene::FileLoadParams{.root = root, .filepath = path});
    p_scene->camera.position = glm::vec3(0, 0, 10.0);
    p_scene->camera.scaling = glm::vec3(1, 1, 1);
    p_scene->camera.zNear = 0.05f;
    p_scene->camera.zFar = 1000.0f;
    p_scene->camera.rotation =
        glm::lookAt(p_scene->camera.position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    for (auto &prop : p_scene->meshProps) {
        prop.transform.position = prop.transform.position * sceneScale;
        prop.transform.scale(glm::vec3(sceneScale));
    }

    Compositor comp(root, p_composeMethod, p_windowFrame);
    comp.parameters.set("scene", p_scene);

    /*
    Render loop!
    */

    while (running) {
        p_rend->render();
        comp.compose();
    }

    /*
    Free resources
    */

    p_rend->free();
    return 0;
}
