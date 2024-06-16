#include <iostream>

#include "Vitrae/ComponentRoot.hpp"
#include "Vitrae/Pipelines/Compositing/Task.hpp"
#include "Vitrae/Pipelines/Shading/Task.hpp"
#include "Vitrae/Visuals/Compositor.hpp"

#include <filesystem>

using namespace Vitrae;

struct AssetCollection
{
    bool running;

    ComponentRoot root;
    Renderer *p_rend;
    dynasma::FirmPtr<FrameStore> p_windowFrame;
    dynasma::FirmPtr<Scene> p_scene;
    Compositor comp;

    AssetCollection(std::filesystem::path scenePath, float sceneScale);
    ~AssetCollection();

    void render();
};